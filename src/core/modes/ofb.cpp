#include "ofb.hpp"

#include "../padding.hpp"
#include "detail.hpp"

namespace shacal::modes {

namespace {

// XOR данных с гаммой OFB на месте. Длина buf обязана быть кратна блоку.
void apply_keystream(std::vector<uint8_t>& buf, const Key& key, const Block& iv,
	const RoundConsts& consts) {
	const Schedule schedule = expand_key(key);
	Block feedback = iv;
	for (std::size_t off = 0; off < buf.size(); off += kBlockBytes) {
		feedback = encrypt_block(feedback, schedule, consts);
		const Block out = detail::xor_block(bytes_to_block(buf.data() + off), feedback);
		block_to_bytes(out, buf.data() + off);
	}
}

}  // namespace

std::vector<uint8_t> ofb_encrypt(const std::vector<uint8_t>& plaintext,
	const Key& key, const Block& iv, const RoundConsts& consts) {
	std::vector<uint8_t> buf = plaintext;
	pkcs7_pad(buf);
	apply_keystream(buf, key, iv, consts);
	return buf;
}

std::vector<uint8_t> ofb_decrypt(const std::vector<uint8_t>& ciphertext,
	const Key& key, const Block& iv, const RoundConsts& consts) {
	detail::require_block_aligned(ciphertext);

	std::vector<uint8_t> buf = ciphertext;
	apply_keystream(buf, key, iv, consts);
	pkcs7_unpad(buf);
	return buf;
}

}  // namespace shacal::modes
