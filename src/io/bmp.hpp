#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

// Слой ввода-вывода BMP. Отделён от ядра: ядро шифрует поток байт и ничего не
// знает про формат изображения.
namespace shacal::io {

#pragma pack(push, 1)
struct BmpFileHeader {
	uint16_t bfType;        // сигнатура 'BM' (0x4D42)
	uint32_t bfSize;
	uint16_t bfReserved1;
	uint16_t bfReserved2;
	uint32_t bfOffBits;
};

struct BmpInfoHeader {
	uint32_t biSize;
	uint32_t biWidth;
	uint32_t biHeight;
	uint16_t biPlanes;
	uint16_t biBitCount;
	uint32_t biCompression;
	uint32_t biSizeImage;
	int32_t biX;
	int32_t biY;
	uint32_t biClrUsed;
	uint32_t biClrImportant;
};
#pragma pack(pop)

inline constexpr uint16_t kBmpMagic = 0x4D42;  // 'B' | 'M' << 8 (little-endian)
inline constexpr std::size_t kBmpHeaderSize = sizeof(BmpFileHeader) + sizeof(BmpInfoHeader);

// Отрезает 54-байтный заголовок BMP от начала data, проверяет сигнатуру и
// возвращает его отдельным буфером. Сам data при этом теряет первые
// kBmpHeaderSize байт (остаются «данные изображения»). Бросает std::runtime_error,
// если данных меньше заголовка или сигнатура неверна.
std::vector<uint8_t> split_bmp_header(std::vector<uint8_t>& data);

}  // namespace shacal::io
