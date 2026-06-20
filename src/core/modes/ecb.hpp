#pragma once

#include <cstdint>
#include <vector>

#include "../shacal1.hpp"

// Режим ECB (Electronic Codebook). Каждый блок шифруется независимо.
// IV не используется. Дополнение - PKCS#7.
namespace shacal::modes {

std::vector<uint8_t> ecb_encrypt(const std::vector<uint8_t>& plaintext,
	const Key& key, const RoundConsts& consts);
std::vector<uint8_t> ecb_decrypt(const std::vector<uint8_t>& ciphertext,
	const Key& key, const RoundConsts& consts);

}  // namespace shacal::modes
