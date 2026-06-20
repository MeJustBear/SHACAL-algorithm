#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include <chrono>

#define KEY_LENGHT 16
#define FULL_KEY_LENGHT 80
#define INT_SIZE 32

#define MOVE_RIGHT_F 24
#define MOVE_RIGHT_S 16
#define MOVE_RIGHT_T 8
#define BLOCKS_SIZE 5
#define BLOCK_SIZE 4
#define CONSTS_SIZE 5
#define ZERO_BLOCK 19

// Режим сцепления блоков. Выбирается в рантайме (флаг --mode)
enum class Mode { ECB, CBC, CFB, OFB };

// Дефолтные раундовые константы SHACAL-1 (как в SHA-1).
// Это НЕ хардкод финального поведения, а лишь значения по умолчанию для штатного
// режима и воспроизведения тест-векторов. На любом уровне API их можно
// переопределить
inline constexpr std::array<uint32_t, 4> kShacal1DefaultRoundConsts = {
	0x5A827999u, 0x6ED9EBA1u, 0x8F1BBCDCu, 0xCA62C1D6u,
};

// Все режимы экспонируют одинаковый интерфейс. Алгоритм работает с потоком байт;
// тип данных (текст/BMP) больше не влияет на логику шифрования. Если нужно
// сохранить «видимым» заголовок BMP - передаётся preserveBmpHeader.
//   key    - 16 × uint32_t (512 бит)
//   iv     - BLOCKS_SIZE × uint32_t (для ECB игнорируется)
//   consts - указатель на 4 × uint32_t (kShacal1DefaultRoundConsts по умолчанию)
namespace ecb {
	void encrypt(const std::string& inFile, const std::string& outFile,
		std::vector<uint32_t>& key, std::vector<uint32_t>& iv,
		const uint32_t* consts, bool preserveBmpHeader, bool stats);
	void decrypt(const std::string& inFile, const std::string& outFile,
		std::vector<uint32_t>& key, std::vector<uint32_t>& iv,
		const uint32_t* consts, bool preserveBmpHeader);
}

namespace cbc {
	void encrypt(const std::string& inFile, const std::string& outFile,
		std::vector<uint32_t>& key, std::vector<uint32_t>& iv,
		const uint32_t* consts, bool preserveBmpHeader, bool stats);
	void decrypt(const std::string& inFile, const std::string& outFile,
		std::vector<uint32_t>& key, std::vector<uint32_t>& iv,
		const uint32_t* consts, bool preserveBmpHeader);
}

namespace cfb {
	void encrypt(const std::string& inFile, const std::string& outFile,
		std::vector<uint32_t>& key, std::vector<uint32_t>& iv,
		const uint32_t* consts, bool preserveBmpHeader, bool stats);
	void decrypt(const std::string& inFile, const std::string& outFile,
		std::vector<uint32_t>& key, std::vector<uint32_t>& iv,
		const uint32_t* consts, bool preserveBmpHeader);
}

namespace ofb {
	void encrypt(const std::string& inFile, const std::string& outFile,
		std::vector<uint32_t>& key, std::vector<uint32_t>& iv,
		const uint32_t* consts, bool preserveBmpHeader, bool stats);
	void decrypt(const std::string& inFile, const std::string& outFile,
		std::vector<uint32_t>& key, std::vector<uint32_t>& iv,
		const uint32_t* consts, bool preserveBmpHeader);
}

// Генерация ключа (KEY_LENGHT слов) и вектора инициализации (BLOCKS_SIZE слов).
std::vector<uint32_t> key_generate();
std::vector<uint32_t> vector_init();

// Чтение/запись последовательности hex-значений (формат: значения через пробел).
// Используется для файлов ключа, IV и совместимого с consts.txt файла констант.
std::vector<uint32_t> read_hex_values(const std::string& filename, size_t count);
void write_hex_values(const std::string& filename, const std::vector<uint32_t>& values);

// Разбор раундовых констант: либо из строки "C0,C1,C2,C3" (--round-consts),
// либо из файла с четырьмя hex-значениями (--round-consts-file).
std::array<uint32_t, 4> parse_round_consts(const std::string& csv);
std::array<uint32_t, 4> read_round_consts_file(const std::string& filename);
