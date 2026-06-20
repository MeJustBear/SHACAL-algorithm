#include "bmp.hpp"

#include <stdexcept>

namespace shacal::io {

std::vector<uint8_t> split_bmp_header(std::vector<uint8_t>& data) {
	if (data.size() < kBmpHeaderSize) {
		throw std::runtime_error("file is too small to contain a BMP header");
	}
	// Сигнатура 'BM' хранится в первых двух байтах (little-endian uint16).
	if (data[0] != 'B' || data[1] != 'M') {
		throw std::runtime_error("file is not BMP");
	}

	std::vector<uint8_t> header(data.begin(), data.begin() + kBmpHeaderSize);
	data.erase(data.begin(), data.begin() + kBmpHeaderSize);
	return header;
}

}  // namespace shacal::io
