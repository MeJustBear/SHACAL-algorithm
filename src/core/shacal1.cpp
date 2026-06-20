#include "shacal1.hpp"

namespace shacal {

uint32_t round_function(uint32_t x, uint32_t y, uint32_t z, unsigned round) {
	if (round < 20u) {
		return (x & y) | (~x & z);
	}
	if ((round >= 20u && round < 40u) || (round >= 60u && round < 80u)) {
		return x ^ y ^ z;
	}
	if (round >= 40u && round < 60u) {
		return (x ^ y) | (x ^ z) | (y ^ z);
	}
	return 0u;
}

Schedule expand_key(const Key& key) {
	Schedule w{};  // нулевая инициализация важна: см. квирк по индексу 16 ниже
	for (unsigned i = 0; i < kRounds; i++) {
		if (i < kKeyWords) {
			w[i] = key[i];
		}
		// Унаследованное поведение исходной реализации: ветка начинается с i > 16,
		// поэтому слово w[16] остаётся нулевым.
		if (i > kKeyWords) {
			w[i] = rotl32(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
		}
	}
	return w;
}

Block encrypt_block(Block block, const Schedule& schedule, const RoundConsts& consts) {
	uint32_t a = block[0];
	uint32_t b = block[1];
	uint32_t c = block[2];
	uint32_t d = block[3];
	uint32_t e = block[4];

	for (unsigned i = 0; i < kRounds; i++) {
		const uint32_t next_a =
			schedule[i] + rotl32(a, 5) + round_function(b, c, d, i) + e + consts[i / 20];
		e = d;
		d = c;
		c = rotl32(b, 30);
		b = a;
		a = next_a;
	}

	return {a, b, c, d, e};
}

Block decrypt_block(Block block, const Schedule& schedule, const RoundConsts& consts) {
	uint32_t a = block[0];
	uint32_t b = block[1];
	uint32_t c = block[2];
	uint32_t d = block[3];
	uint32_t e = block[4];

	for (unsigned i = 0; i < kRounds; i++) {
		const unsigned round = kRounds - 1u - i;
		const uint32_t next_b = rotl32(c, 2);
		// Вычитания по модулю 2^32 выражены через сложение с побитовым НЕ (~),
		// плюс константа 4 - как в исходной decrypt_block.
		const uint32_t next_e =
			~schedule[round] + ~rotl32(b, 5) + ~round_function(next_b, d, e, round) +
			a + ~consts[round / 20] + 4u;
		a = b;
		b = next_b;
		c = d;
		d = e;
		e = next_e;
	}

	return {a, b, c, d, e};
}

Block bytes_to_block(const uint8_t* p) {
	Block block{};
	for (std::size_t i = 0; i < kBlockWords; i++) {
		block[i] = load_be32(p + i * 4);
	}
	return block;
}

void block_to_bytes(const Block& block, uint8_t* p) {
	for (std::size_t i = 0; i < kBlockWords; i++) {
		store_be32(p + i * 4, block[i]);
	}
}

}  // namespace shacal
