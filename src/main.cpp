#include "Header.h"

#include <array>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

namespace {

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
		"Раундовые константы: если не заданы ни --round-consts, ни --round-consts-file,\n"
		"используются встроенные дефолты SHACAL-1. При расшифровании набор констант\n"
		"должен совпадать с использованным при шифровании.\n";
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

array<uint32_t, 4> resolve_round_consts(const Options& opt) {
	require(opt.roundConstsCsv.empty() || opt.roundConstsFile.empty(),
		"--round-consts and --round-consts-file are mutually exclusive");
	if (!opt.roundConstsCsv.empty()) return parse_round_consts(opt.roundConstsCsv);
	if (!opt.roundConstsFile.empty()) return read_round_consts_file(opt.roundConstsFile);
	return kShacal1DefaultRoundConsts;
}

void run_cipher(const Options& opt, bool encrypting) {
	require(!opt.keyFile.empty(), "--key is required");
	require(!opt.inFile.empty(), "-i/--in is required");
	require(!opt.outFile.empty(), "-o/--out is required");

	array<uint32_t, 4> consts = resolve_round_consts(opt);

	vector<uint32_t> key = read_hex_values(opt.keyFile, KEY_LENGHT);

	vector<uint32_t> iv;
	if (opt.mode == Mode::ECB) {
		iv.assign(BLOCKS_SIZE, 0);
	} else {
		require(!opt.ivFile.empty(), "--iv is required for cbc/cfb/ofb modes");
		iv = read_hex_values(opt.ivFile, BLOCKS_SIZE);
	}

	const uint32_t* c = consts.data();
	switch (opt.mode) {
	case Mode::ECB:
		if (encrypting) ecb::encrypt(opt.inFile, opt.outFile, key, iv, c, opt.preserveBmp, opt.stats);
		else ecb::decrypt(opt.inFile, opt.outFile, key, iv, c, opt.preserveBmp);
		break;
	case Mode::CBC:
		if (encrypting) cbc::encrypt(opt.inFile, opt.outFile, key, iv, c, opt.preserveBmp, opt.stats);
		else cbc::decrypt(opt.inFile, opt.outFile, key, iv, c, opt.preserveBmp);
		break;
	case Mode::CFB:
		if (encrypting) cfb::encrypt(opt.inFile, opt.outFile, key, iv, c, opt.preserveBmp, opt.stats);
		else cfb::decrypt(opt.inFile, opt.outFile, key, iv, c, opt.preserveBmp);
		break;
	case Mode::OFB:
		if (encrypting) ofb::encrypt(opt.inFile, opt.outFile, key, iv, c, opt.preserveBmp, opt.stats);
		else ofb::decrypt(opt.inFile, opt.outFile, key, iv, c, opt.preserveBmp);
		break;
	}
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
			write_hex_values(opt.outFile, key_generate());
			cout << "key written to " << opt.outFile << '\n';
		} else if (opt.command == "ivgen") {
			require(!opt.outFile.empty(), "-o/--out is required for ivgen");
			write_hex_values(opt.outFile, vector_init());
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
