#pragma once

#include <cstdint>
#include <vector>

#include "../shacal1.hpp"

// Режим CFB (Cipher Feedback), полноблочный вариант. Гамма получается
// шифрованием предыдущего шифрблока (для первого - IV). Дополнение - PKCS#7
// (для единообразия со всеми режимами).
namespace shacal::modes {

std::vector<uint8_t> cfb_encrypt(const std::vector<uint8_t>& plaintext,
	const Key& key, const Block& iv, const RoundConsts& consts);
std::vector<uint8_t> cfb_decrypt(const std::vector<uint8_t>& ciphertext,
	const Key& key, const Block& iv, const RoundConsts& consts);

}  // namespace shacal::modes
