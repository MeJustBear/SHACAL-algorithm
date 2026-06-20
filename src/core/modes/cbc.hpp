#pragma once

#include <cstdint>
#include <vector>

#include "../shacal1.hpp"

// Режим CBC (Cipher Block Chaining). Блок XOR-ится с предыдущим шифрблоком
// (для первого блока - с IV) перед шифрованием. Дополнение - PKCS#7.
namespace shacal::modes {

std::vector<uint8_t> cbc_encrypt(const std::vector<uint8_t>& plaintext,
	const Key& key, const Block& iv, const RoundConsts& consts);
std::vector<uint8_t> cbc_decrypt(const std::vector<uint8_t>& ciphertext,
	const Key& key, const Block& iv, const RoundConsts& consts);

}  // namespace shacal::modes
