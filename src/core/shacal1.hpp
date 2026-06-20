#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

// Чистое алгоритмическое ядро SHACAL-1.
//
// Слой core не знает про файлы, BMP и std::cout: он работает только с буферами
// в памяти и сообщает об ошибках через исключения из errors.hpp. Раундовые
// константы передаются в каждый вызов отдельным аргументом - ядро никогда не
// читает consts.txt и не зашивает единственный набор внутрь раундовой функции
// (см. docs/roadmap.md, раздел про раундовые константы).
namespace shacal {

inline constexpr std::size_t kBlockWords = 5;    // 160-битный блок
inline constexpr std::size_t kBlockBytes = 20;   // = kBlockWords * 4
inline constexpr std::size_t kKeyWords = 16;     // 512-битный ключ
inline constexpr std::size_t kRounds = 80;       // развёрнутый ключ
inline constexpr std::size_t kRoundConsts = 4;   // по одной на 20 раундов

using Block = std::array<uint32_t, kBlockWords>;
using Key = std::array<uint32_t, kKeyWords>;
using Schedule = std::array<uint32_t, kRounds>;
using RoundConsts = std::array<uint32_t, kRoundConsts>;

// Дефолтные раундовые константы SHACAL-1 (как в SHA-1). Это НЕ хардкод финального
// поведения, а лишь значения по умолчанию; на любом уровне API их можно
// переопределить.
inline constexpr RoundConsts kDefaultRoundConsts = {
	0x5A827999u, 0x6ED9EBA1u, 0x8F1BBCDCu, 0xCA62C1D6u,
};

// Чтение/запись 32-битного слова в формате big-endian. Заменяют прежнюю ручную
// побайтовую сборку uint32_t (старые read_block / write_output).
inline uint32_t load_be32(const uint8_t* p) {
	return (static_cast<uint32_t>(p[0]) << 24) |
	       (static_cast<uint32_t>(p[1]) << 16) |
	       (static_cast<uint32_t>(p[2]) << 8) |
	       static_cast<uint32_t>(p[3]);
}

inline void store_be32(uint8_t* p, uint32_t value) {
	p[0] = static_cast<uint8_t>(value >> 24);
	p[1] = static_cast<uint8_t>(value >> 16);
	p[2] = static_cast<uint8_t>(value >> 8);
	p[3] = static_cast<uint8_t>(value);
}

// Циклический сдвиг (ROTL) 32-битного значения влево на bits бит.
inline uint32_t rotl32(uint32_t value, unsigned bits) {
	bits &= 31u;
	if (bits == 0u) {
		return value;
	}
	return (value << bits) | (value >> (32u - bits));
}

// Раундовая логическая функция SHACAL-1, выбираемая по номеру раунда:
//   0–19      - Ch:     (x & y) | (~x & z)
//   20–39, 60–79 - Parity: x ^ y ^ z
//   40–59     - Maj:    (x & y) | (x & z) | (y & z)
uint32_t round_function(uint32_t x, uint32_t y, uint32_t z, unsigned round);

// Развёртка ключа (key schedule): расширяет 16 слов исходного ключа до 80
// раундовых слов по правилу SHACAL-1/SHA-1.
Schedule expand_key(const Key& key);

// Шифрование/расшифрование одного 160-битного блока (5 слов) за 80 раундов.
// schedule - результат expand_key, consts - 4 раундовые константы.
Block encrypt_block(Block block, const Schedule& schedule, const RoundConsts& consts);
Block decrypt_block(Block block, const Schedule& schedule, const RoundConsts& consts);

// Упаковка/распаковка одного блока из/в непрерывный буфер байт (big-endian).
Block bytes_to_block(const uint8_t* p);
void block_to_bytes(const Block& block, uint8_t* p);

}  // namespace shacal
