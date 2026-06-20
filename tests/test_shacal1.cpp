// Тесты блочного примитива SHACAL-1: be32-конверсия, ROTL, key schedule,
// round-trip блока и зафиксированный (golden) тест-вектор.
//
// Важно: реализация намеренно сохраняет унаследованный квирк key schedule
// (слово w[16] остаётся нулевым, см. комментарий в expand_key). Поэтому здесь
// проверяются НЕ официальные тест-векторы SHACAL-1, а векторы, зафиксированные
// из самой реализации, - они служат якорем регрессии («поведение не изменилось»).

#include <catch2/catch_test_macros.hpp>

#include <array>
#include <cstdint>

#include "common.hpp"
#include "shacal1.hpp"

using namespace shacal;

TEST_CASE("shacal1: load_be32 / store_be32 on known values", "[shacal1][be32]") {
	const uint8_t bytes[4] = {0xDE, 0xAD, 0xBE, 0xEF};
	REQUIRE(load_be32(bytes) == 0xDEADBEEFu);

	uint8_t out[4] = {0, 0, 0, 0};
	store_be32(out, 0x01020304u);
	REQUIRE(out[0] == 0x01);
	REQUIRE(out[1] == 0x02);
	REQUIRE(out[2] == 0x03);
	REQUIRE(out[3] == 0x04);

	SECTION("store then load is identity") {
		store_be32(out, 0x89ABCDEFu);
		REQUIRE(load_be32(out) == 0x89ABCDEFu);
	}
}

TEST_CASE("shacal1: rotl32 cyclic shift", "[shacal1][rotl]") {
	REQUIRE(rotl32(0x00000001u, 0) == 0x00000001u);   // сдвиг на 0 не UB
	REQUIRE(rotl32(0x80000000u, 1) == 0x00000001u);   // старший бит уходит в младший
	REQUIRE(rotl32(0x12345678u, 32) == 0x12345678u);  // сдвиг на 32 == 0
	REQUIRE(rotl32(0x12345678u, 8) == 0x34567812u);
}

TEST_CASE("shacal1: expand_key 16 -> 80 words and the inherited w[16] quirk", "[shacal1][schedule]") {
	const Key key = test::fixed_key();
	const Schedule w = expand_key(key);

	SECTION("first 16 words equal the key") {
		for (std::size_t i = 0; i < kKeyWords; i++) {
			REQUIRE(w[i] == key[i]);
		}
	}

	SECTION("w[16] stays zero (intentionally preserved quirk)") {
		REQUIRE(w[16] == 0u);
	}

	SECTION("words i>16 follow the SHA-1 recurrence") {
		for (std::size_t i = 17; i < kRounds; i++) {
			const uint32_t expected =
				rotl32(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
			REQUIRE(w[i] == expected);
		}
	}
}

TEST_CASE("shacal1: encrypt_block / decrypt_block single-block round-trip", "[shacal1][block]") {
	const Schedule schedule = expand_key(test::fixed_key());

	Block pt = {0x00010203u, 0x04050607u, 0x08090A0Bu, 0x0C0D0E0Fu, 0x10111213u};
	const Block ct = encrypt_block(pt, schedule, kDefaultRoundConsts);
	const Block back = decrypt_block(ct, schedule, kDefaultRoundConsts);

	REQUIRE(back == pt);
	REQUIRE(ct != pt);  // шифрование действительно меняет блок
}

TEST_CASE("shacal1: golden block-transform vector (regression)", "[shacal1][vector]") {
	// Значение зафиксировано из текущей реализации; меняться не должно без
	// осознанного решения (иначе ломается совместимость с прежним выходом).
	const Schedule schedule = expand_key(test::fixed_key());
	const Block pt = {0x00010203u, 0x04050607u, 0x08090A0Bu, 0x0C0D0E0Fu, 0x10111213u};
	const Block ct = encrypt_block(pt, schedule, kDefaultRoundConsts);

	const Block expected = {0xD4B0A13Eu, 0xB74DD153u, 0x88D1C00Du, 0x8AC7AB1Fu, 0x8DD437FDu};
	REQUIRE(ct == expected);
}

TEST_CASE("shacal1: block round-trip with custom round constants", "[shacal1][consts]") {
	const Schedule schedule = expand_key(test::fixed_key());
	const RoundConsts custom = {0x11111111u, 0x22222222u, 0x33333333u, 0x44444444u};

	const Block pt = {0xDEADBEEFu, 0xCAFEBABEu, 0x01234567u, 0x89ABCDEFu, 0x0BADF00Du};
	const Block ct = encrypt_block(pt, schedule, custom);
	REQUIRE(decrypt_block(ct, schedule, custom) == pt);

	SECTION("a different constants set yields a different ciphertext") {
		const Block ct_default = encrypt_block(pt, schedule, kDefaultRoundConsts);
		REQUIRE(ct != ct_default);
	}
}
