#include "cfb.hpp"

#include "../padding.hpp"
#include "detail.hpp"

namespace shacal::modes {

std::vector<uint8_t> cfb_encrypt(const std::vector<uint8_t>& plaintext,
	const Key& key, const Block& iv, const RoundConsts& consts) {
	std::vector<uint8_t> buf = plaintext;
	pkcs7_pad(buf);

	const Schedule schedule = expand_key(key);
	Block chain = iv;
	for (std::size_t off = 0; off < buf.size(); off += kBlockBytes) {
		const Block keystream = encrypt_block(chain, schedule, consts);
		const Block cipher = detail::xor_block(bytes_to_block(buf.data() + off), keystream);
		chain = cipher;
		block_to_bytes(cipher, buf.data() + off);
	}
	return buf;
}

std::vector<uint8_t> cfb_decrypt(const std::vector<uint8_t>& ciphertext,
	const Key& key, const Block& iv, const RoundConsts& consts) {
	detail::require_block_aligned(ciphertext);

	std::vector<uint8_t> buf = ciphertext;
	const Schedule schedule = expand_key(key);
	Block chain = iv;
	for (std::size_t off = 0; off < buf.size(); off += kBlockBytes) {
		const Block cipher = bytes_to_block(buf.data() + off);
		const Block keystream = encrypt_block(chain, schedule, consts);
		const Block plain = detail::xor_block(cipher, keystream);
		chain = cipher;
		block_to_bytes(plain, buf.data() + off);
	}

	pkcs7_unpad(buf);
	return buf;
}

}  // namespace shacal::modes
