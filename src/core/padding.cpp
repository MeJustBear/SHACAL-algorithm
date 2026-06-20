#include "padding.hpp"

#include "errors.hpp"

namespace shacal {

void pkcs7_pad(std::vector<uint8_t>& data, std::size_t block) {
	if (block == 0 || block > 255) {
		throw InvalidArgument("PKCS#7 block size must be in 1..255");
	}
	const std::size_t pad = block - (data.size() % block);  // 1..block
	data.insert(data.end(), pad, static_cast<uint8_t>(pad));
}

void pkcs7_unpad(std::vector<uint8_t>& data, std::size_t block) {
	if (block == 0 || block > 255) {
		throw InvalidArgument("PKCS#7 block size must be in 1..255");
	}
	if (data.empty() || data.size() % block != 0) {
		throw PaddingError("PKCS#7: data length is not a multiple of the block size");
	}

	const std::size_t pad = data.back();
	if (pad == 0 || pad > block) {
		throw PaddingError("PKCS#7: pad length out of range");
	}
	for (std::size_t i = 0; i < pad; i++) {
		if (data[data.size() - 1 - i] != pad) {
			throw PaddingError("PKCS#7: inconsistent padding bytes");
		}
	}

	data.resize(data.size() - pad);
}

}  // namespace shacal
