#pragma once

#include <cstdint>
#include <vector>

// Исследовательские метрики статистических свойств шифра. Печатают в std::cout,
// поэтому живут в CLI-слое, а не в чистом ядре.
namespace shacal::cli {

// Преобразует буфер байт в слова uint32_t (big-endian). Хвост короче 4 байт
// игнорируется (на практике входы кратны блоку).
std::vector<uint32_t> bytes_to_words_be(const std::vector<uint8_t>& bytes);

// Распределение нулевых и единичных бит - простая оценка равномерности выхода.
void dispersion(const std::vector<uint32_t>& data);

// Корреляция между входом и выходом - оценка статистической связи открытого
// текста с шифртекстом.
void correlation(const std::vector<uint32_t>& in, const std::vector<uint32_t>& out);

}  // namespace shacal::cli
