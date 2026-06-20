#include <array>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "../core/modes/cbc.hpp"
#include "../core/modes/cfb.hpp"
#include "../core/modes/ecb.hpp"
#include "../core/modes/ofb.hpp"
#include "../core/padding.hpp"
#include "../core/shacal1.hpp"
#include "../io/bmp.hpp"
#include "../io/streams.hpp"
#include "stats.hpp"

using namespace std;
using namespace shacal;

namespace {

enum class Mode { ECB, CBC, CFB, OFB };

struct Options {
	string command;
	Mode mode = Mode::ECB;
	string keyFile;
	string ivFile;
	string inFile;
	string outFile;
	string roundConstsCsv;
	string roundConstsFile;
	bool preserveBmp = false;
	bool stats = false;
};

void print_usage() {
	cout <<
		"SHACAL-1 block cipher CLI (modes: ecb, cbc, cfb, ofb)\n\n"
		"Usage:\n"
		"  shacal_cli encrypt --mode <ecb|cbc|cfb|ofb> --key <file> [--iv <file>]\n"
		"                     -i <in> -o <out> [round-consts] [--preserve-bmp-header] [--stats]\n"
		"  shacal_cli decrypt --mode <ecb|cbc|cfb|ofb> --key <file> [--iv <file>]\n"
		"                     -i <in> -o <out> [round-consts] [--preserve-bmp-header]\n"
		"  shacal_cli keygen  --out <file>\n"
		"  shacal_cli ivgen   --out <file>\n\n"
		"Options:\n"
		"  --mode <name>            режим сцепления блоков (по умолчанию ecb)\n"
		"  --key <file>             файл ключа (16 hex-значений uint32, через пробел)\n"
		"  --iv <file>              файл вектора инициализации (5 hex-значений; для cbc/cfb/ofb)\n"
		"  -i, --in <file>          входной файл\n"
		"  -o, --out <file>         выходной файл\n"
		"  --round-consts C0,C1,C2,C3   4 раундовые константы в hex (напр. 0x5A827999,...)\n"
		"  --round-consts-file <f>  файл с 4 hex-константами (совместим со старым consts.txt)\n"
		"  --preserve-bmp-header    сохранить 54-байтный заголовок BMP \"видимым\"\n"
		"  --stats                  вывести дисперсию/корреляцию (только encrypt)\n"
		"  -h, --help               показать эту справку\n\n"
		"Дополнение (padding): PKCS#7. При расшифровании набор раундовых констант,\n"
		"ключ и IV должны совпадать с использованными при шифровании, иначе будет\n"
		"ошибка некорректного дополнения.\n";
}

Mode parse_mode(const string& value) {
	if (value == "ecb") return Mode::ECB;
	if (value == "cbc") return Mode::CBC;
	if (value == "cfb") return Mode::CFB;
	if (value == "ofb") return Mode::OFB;
	throw runtime_error("unknown mode: " + value + " (expected ecb|cbc|cfb|ofb)");
}

void require(bool condition, const string& message) {
	if (!condition) {
		throw runtime_error(message);
	}
}

RoundConsts parse_round_consts_csv(const string& csv) {
	RoundConsts consts{};
	stringstream ss(csv);
	string token;
	size_t i = 0;
	while (getline(ss, token, ',') && i < consts.size()) {
		consts[i++] = static_cast<uint32_t>(stoul(token, nullptr, 16));
	}
	if (i != consts.size()) {
		throw runtime_error("--round-consts expects 4 comma-separated hex values");
	}
	return consts;
}

RoundConsts resolve_round_consts(const Options& opt) {
	require(opt.roundConstsCsv.empty() || opt.roundConstsFile.empty(),
		"--round-consts and --round-consts-file are mutually exclusive");
	if (!opt.roundConstsCsv.empty()) {
		return parse_round_consts_csv(opt.roundConstsCsv);
	}
	if (!opt.roundConstsFile.empty()) {
		vector<uint32_t> values = io::read_hex_values(opt.roundConstsFile, kRoundConsts);
		return {values[0], values[1], values[2], values[3]};
	}
	return kDefaultRoundConsts;
}

template <std::size_t N>
array<uint32_t, N> to_array(const vector<uint32_t>& values) {
	array<uint32_t, N> out{};
	for (size_t i = 0; i < N; i++) {
		out[i] = values[i];
	}
	return out;
}

vector<uint32_t> fill_random_vector(size_t count) {
	vector<uint32_t> values(count);
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_int_distribution<uint32_t> distribution(
		std::numeric_limits<uint32_t>::min(),
		std::numeric_limits<uint32_t>::max());
	for (size_t i = 0; i < count; i++) {
		values[i] = distribution(generator);
	}
	return values;
}

vector<uint8_t> run_mode(Mode mode, bool encrypting, const vector<uint8_t>& data,
	const Key& key, const Block& iv, const RoundConsts& consts) {
	switch (mode) {
	case Mode::ECB:
		return encrypting ? modes::ecb_encrypt(data, key, consts)
		                  : modes::ecb_decrypt(data, key, consts);
	case Mode::CBC:
		return encrypting ? modes::cbc_encrypt(data, key, iv, consts)
		                  : modes::cbc_decrypt(data, key, iv, consts);
	case Mode::CFB:
		return encrypting ? modes::cfb_encrypt(data, key, iv, consts)
		                  : modes::cfb_decrypt(data, key, iv, consts);
	case Mode::OFB:
		return encrypting ? modes::ofb_encrypt(data, key, iv, consts)
		                  : modes::ofb_decrypt(data, key, iv, consts);
	}
	throw runtime_error("unreachable: invalid mode");
}

void run_cipher(const Options& opt, bool encrypting) {
	require(!opt.keyFile.empty(), "--key is required");
	require(!opt.inFile.empty(), "-i/--in is required");
	require(!opt.outFile.empty(), "-o/--out is required");

	const RoundConsts consts = resolve_round_consts(opt);
	const Key key = to_array<kKeyWords>(io::read_hex_values(opt.keyFile, kKeyWords));

	Block iv{};
	if (opt.mode != Mode::ECB) {
		require(!opt.ivFile.empty(), "--iv is required for cbc/cfb/ofb modes");
		iv = to_array<kBlockWords>(io::read_hex_values(opt.ivFile, kBlockWords));
	}

	vector<uint8_t> data = io::read_file(opt.inFile);

	// При --preserve-bmp-header первые 54 байта (заголовок BMP) не шифруются:
	// отделяем их до обработки и дописываем обратно в начало результата.
	vector<uint8_t> bmpHeader;
	if (opt.preserveBmp) {
		bmpHeader = io::split_bmp_header(data);
	}

	vector<uint8_t> processed = run_mode(opt.mode, encrypting, data, key, iv, consts);

	if (opt.stats && encrypting) {
		vector<uint8_t> padded = data;
		pkcs7_pad(padded);
		const vector<uint32_t> inputWords = cli::bytes_to_words_be(padded);
		const vector<uint32_t> outputWords = cli::bytes_to_words_be(processed);
		cli::correlation(inputWords, outputWords);
		cli::dispersion(outputWords);
	}

	vector<uint8_t> output;
	output.reserve(bmpHeader.size() + processed.size());
	output.insert(output.end(), bmpHeader.begin(), bmpHeader.end());
	output.insert(output.end(), processed.begin(), processed.end());
	io::write_file(opt.outFile, output);
}

}  // namespace

int main(int argc, char** argv) {
	try {
		if (argc < 2) {
			print_usage();
			return 1;
		}

		Options opt;
		opt.command = argv[1];
		if (opt.command == "-h" || opt.command == "--help") {
			print_usage();
			return 0;
		}

		for (int i = 2; i < argc; i++) {
			string flag = argv[i];
			auto next = [&]() -> string {
				if (i + 1 >= argc) throw runtime_error("missing value for " + flag);
				return argv[++i];
			};
			if (flag == "--mode") opt.mode = parse_mode(next());
			else if (flag == "--key") opt.keyFile = next();
			else if (flag == "--iv") opt.ivFile = next();
			else if (flag == "-i" || flag == "--in") opt.inFile = next();
			else if (flag == "-o" || flag == "--out") opt.outFile = next();
			else if (flag == "--round-consts") opt.roundConstsCsv = next();
			else if (flag == "--round-consts-file") opt.roundConstsFile = next();
			else if (flag == "--preserve-bmp-header") opt.preserveBmp = true;
			else if (flag == "--stats") opt.stats = true;
			else if (flag == "-h" || flag == "--help") { print_usage(); return 0; }
			else throw runtime_error("unknown option: " + flag);
		}

		if (opt.command == "encrypt") {
			run_cipher(opt, true);
		} else if (opt.command == "decrypt") {
			run_cipher(opt, false);
		} else if (opt.command == "keygen") {
			require(!opt.outFile.empty(), "-o/--out is required for keygen");
			io::write_hex_values(opt.outFile, fill_random_vector(kKeyWords));
			cout << "key written to " << opt.outFile << '\n';
		} else if (opt.command == "ivgen") {
			require(!opt.outFile.empty(), "-o/--out is required for ivgen");
			io::write_hex_values(opt.outFile, fill_random_vector(kBlockWords));
			cout << "iv written to " << opt.outFile << '\n';
		} else {
			throw runtime_error("unknown command: " + opt.command +
				" (expected encrypt|decrypt|keygen|ivgen)");
		}
	} catch (const exception& e) {
		cerr << "error: " << e.what() << '\n';
		return 1;
	}

	return 0;
}
