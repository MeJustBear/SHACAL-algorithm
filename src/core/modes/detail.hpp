#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "../errors.hpp"
#include "../shacal1.hpp"

// Внутренние помощники для реализации режимов сцепления. Не часть публичного API.
namespace shacal::modes::detail {

inline Block xor_block(const Block& a, const Block& b) {
	Block out{};
	for (std::size_t i = 0; i < kBlockWords; i++) {
		out[i] = a[i] ^ b[i];
	}
	return out;
}

// Проверка, что длина шифртекста кратна размеру блока (обязательное условие для
// расшифрования любого режима). Иначе данные заведомо повреждены.
inline void require_block_aligned(const std::vector<uint8_t>& ciphertext) {
	if (ciphertext.empty() || ciphertext.size() % kBlockBytes != 0) {
		throw PaddingError("ciphertext length must be a positive multiple of the block size");
	}
}

}  // namespace shacal::modes::detail
