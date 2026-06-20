#include "ecb.hpp"

#include "../padding.hpp"
#include "detail.hpp"

namespace shacal::modes {

std::vector<uint8_t> ecb_encrypt(const std::vector<uint8_t>& plaintext,
	const Key& key, const RoundConsts& consts) {
	std::vector<uint8_t> buf = plaintext;
	pkcs7_pad(buf);

	const Schedule schedule = expand_key(key);
	for (std::size_t off = 0; off < buf.size(); off += kBlockBytes) {
		Block block = bytes_to_block(buf.data() + off);
		block = encrypt_block(block, schedule, consts);
		block_to_bytes(block, buf.data() + off);
	}
	return buf;
}

std::vector<uint8_t> ecb_decrypt(const std::vector<uint8_t>& ciphertext,
	const Key& key, const RoundConsts& consts) {
	detail::require_block_aligned(ciphertext);

	std::vector<uint8_t> buf = ciphertext;
	const Schedule schedule = expand_key(key);
	for (std::size_t off = 0; off < buf.size(); off += kBlockBytes) {
		Block block = bytes_to_block(buf.data() + off);
		block = decrypt_block(block, schedule, consts);
		block_to_bytes(block, buf.data() + off);
	}

	pkcs7_unpad(buf);
	return buf;
}

}  // namespace shacal::modes
