#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

// Файловый ввод-вывод: чтение/запись «сырых» байт и текстовых hex-файлов
// (ключ, IV, набор раундовых констант). Алгоритмическое ядро этих функций не видит.
namespace shacal::io {

// Чтение всего файла в буфер байт (бинарный режим).
std::vector<uint8_t> read_file(const std::string& path);

// Запись буфера байт в файл (бинарный режим).
void write_file(const std::string& path, const std::vector<uint8_t>& data);

// Чтение ровно count шестнадцатеричных значений uint32_t (через пробелы).
// Формат совместим с прежними файлами key.txt / iv.txt / consts.txt.
std::vector<uint32_t> read_hex_values(const std::string& path, std::size_t count);

// Запись последовательности hex-значений (через пробел).
void write_hex_values(const std::string& path, const std::vector<uint32_t>& values);

}  // namespace shacal::io
