// End-to-end тест соответствия CLI эталону (roadmap, Этап 4: «совпадение с
// эталоном»). Запускает собранный бинарник shacal_cli на временных файлах и
// проверяет, что:
//  - выход CLI для фиксированных ключа/IV/входа совпадает с golden-векторами
//    (тем же, что и у core) - значит слой CLI ничего не искажает;
//  - round-trip encrypt -> decrypt через файлы возвращает исходные данные.

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "common.hpp"

using namespace shacal;
namespace fs = std::filesystem;

namespace {

#ifndef SHACAL_CLI_PATH
#define SHACAL_CLI_PATH "shacal_cli"
#endif

void write_bytes(const fs::path& p, const std::vector<uint8_t>& data) {
	std::ofstream out(p, std::ios::binary);
	out.write(reinterpret_cast<const char*>(data.data()),
		static_cast<std::streamsize>(data.size()));
}

void write_text(const fs::path& p, const std::string& text) {
	std::ofstream out(p);
	out << text;
}

std::vector<uint8_t> read_bytes(const fs::path& p) {
	std::ifstream in(p, std::ios::binary);
	return std::vector<uint8_t>(std::istreambuf_iterator<char>(in),
		std::istreambuf_iterator<char>());
}

// Запуск CLI; возвращает код возврата процесса.
int run_cli(const std::string& args) {
	std::string cmd = std::string("\"") + SHACAL_CLI_PATH + "\" " + args;
#ifdef _WIN32
	// cmd.exe /c срезает внешнюю пару кавычек, если строка и начинается, и
	// заканчивается кавычкой (а у нас так из-за кавычек вокруг путей). Из-за
	// этого ломаются внутренние кавычки. Обходим, обернув всю команду ещё одной
	// парой кавычек — её cmd.exe и «съест», сохранив внутренние.
	cmd = "\"" + cmd + "\"";
#endif
	return std::system(cmd.c_str());
}

// Hex-представление fixed_key/fixed_iv в формате, который читает CLI
// (значения uint32 через пробел).
std::string words_hex(const std::array<uint32_t, kKeyWords>& key) {
	std::string s;
	char buf[16];
	for (std::size_t i = 0; i < key.size(); i++) {
		std::snprintf(buf, sizeof(buf), "%08x", key[i]);
		if (i) s += ' ';
		s += buf;
	}
	return s;
}

std::string words_hex(const Block& iv) {
	std::string s;
	char buf[16];
	for (std::size_t i = 0; i < iv.size(); i++) {
		std::snprintf(buf, sizeof(buf), "%08x", iv[i]);
		if (i) s += ' ';
		s += buf;
	}
	return s;
}

}  // namespace

TEST_CASE("cli: output matches the core golden vectors", "[cli][vectors]") {
	const fs::path dir = fs::temp_directory_path() / "shacal_cli_vec_test";
	fs::create_directories(dir);

	const fs::path keyFile = dir / "key.txt";
	const fs::path ivFile = dir / "iv.txt";
	const fs::path inFile = dir / "in.bin";
	const fs::path outFile = dir / "out.bin";

	write_text(keyFile, words_hex(test::fixed_key()));
	write_text(ivFile, words_hex(test::fixed_iv()));
	write_bytes(inFile, test::incrementing(50));

	SECTION("ecb") {
		const int rc = run_cli("encrypt --mode ecb --key \"" + keyFile.string() +
			"\" -i \"" + inFile.string() + "\" -o \"" + outFile.string() + "\"");
		REQUIRE(rc == 0);
		REQUIRE(test::to_hex(read_bytes(outFile)) ==
			"d4b0a13eb74dd15388d1c00d8ac7ab1f8dd437fdf8847942699be438ea15b8fb"
			"84f1bb13cc9592bc4798a55717390f2822cd75eff0a279a307fad7ec");
	}

	SECTION("cbc") {
		const int rc = run_cli("encrypt --mode cbc --key \"" + keyFile.string() +
			"\" --iv \"" + ivFile.string() + "\" -i \"" + inFile.string() +
			"\" -o \"" + outFile.string() + "\"");
		REQUIRE(rc == 0);
		REQUIRE(test::to_hex(read_bytes(outFile)) ==
			"44f0a4a8806c6e8205bdbcf79d382ba8f00c57fdbd452401d5d91119d914e526"
			"d37f26e204dc89fac17c1284aa1893428f4181c0774ab7548c88db48");
	}

	fs::remove_all(dir);
}

TEST_CASE("cli: round-trip encrypt -> decrypt through files", "[cli][roundtrip]") {
	const fs::path dir = fs::temp_directory_path() / "shacal_cli_rt_test";
	fs::create_directories(dir);

	const fs::path keyFile = dir / "key.txt";
	const fs::path ivFile = dir / "iv.txt";
	const fs::path inFile = dir / "in.bin";
	const fs::path encFile = dir / "out.enc";
	const fs::path decFile = dir / "out.dec";

	write_text(keyFile, words_hex(test::fixed_key()));
	write_text(ivFile, words_hex(test::fixed_iv()));
	const std::vector<uint8_t> original = test::incrementing(57);  // невыровненная длина
	write_bytes(inFile, original);

	const std::string base = " --mode cbc --key \"" + keyFile.string() +
		"\" --iv \"" + ivFile.string() + "\"";

	REQUIRE(run_cli("encrypt" + base + " -i \"" + inFile.string() +
		"\" -o \"" + encFile.string() + "\"") == 0);
	REQUIRE(run_cli("decrypt" + base + " -i \"" + encFile.string() +
		"\" -o \"" + decFile.string() + "\"") == 0);

	REQUIRE(read_bytes(decFile) == original);

	fs::remove_all(dir);
}
