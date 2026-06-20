#pragma once

#include <cstdint>
#include <vector>

#include "../shacal1.hpp"

// Режим OFB (Output Feedback). Гамма получается итеративным шифрованием IV:
// k1 = E(IV), k2 = E(k1), ... Шифрование и расшифрование симметричны.
// Дополнение - PKCS#7 (для единообразия со всеми режимами).
namespace shacal::modes {

std::vector<uint8_t> ofb_encrypt(const std::vector<uint8_t>& plaintext,
	const Key& key, const Block& iv, const RoundConsts& consts);
std::vector<uint8_t> ofb_decrypt(const std::vector<uint8_t>& ciphertext,
	const Key& key, const Block& iv, const RoundConsts& consts);

}  // namespace shacal::modes
