#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "shacal1.hpp"

// Общие для тестов фиксированные входы и помощники.
namespace shacal::test {

// Детерминированный ключ: слово i состоит из байт (4i, 4i+1, 4i+2, 4i+3),
// т.е. весь ключ - это байты 0x00..0x3F. Легко воспроизвести вручную.
inline Key fixed_key() {
	Key key{};
	for (std::size_t i = 0; i < kKeyWords; i++) {
		key[i] = (uint32_t(4 * i) << 24) | (uint32_t(4 * i + 1) << 16) |
		         (uint32_t(4 * i + 2) << 8) | uint32_t(4 * i + 3);
	}
	return key;
}

// Фиксированный вектор инициализации.
inline Block fixed_iv() {
	return {0xA0A1A2A3u, 0xA4A5A6A7u, 0xA8A9AAABu, 0xACADAEAFu, 0xB0B1B2B3u};
}

// Сообщение из n инкрементных байт (0, 1, 2, ... mod 256).
inline std::vector<uint8_t> incrementing(std::size_t n) {
	std::vector<uint8_t> v(n);
	for (std::size_t i = 0; i < n; i++) {
		v[i] = static_cast<uint8_t>(i);
	}
	return v;
}

// Перевод буфера байт в hex-строку (нижний регистр, без разделителей).
inline std::string to_hex(const std::vector<uint8_t>& v) {
	static const char* digits = "0123456789abcdef";
	std::string out;
	out.reserve(v.size() * 2);
	for (uint8_t b : v) {
		out.push_back(digits[b >> 4]);
		out.push_back(digits[b & 0x0F]);
	}
	return out;
}

}  // namespace shacal::test
