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

TEST_CASE("load_be32 / store_be32 на известных значениях", "[shacal1][be32]") {
	const uint8_t bytes[4] = {0xDE, 0xAD, 0xBE, 0xEF};
	REQUIRE(load_be32(bytes) == 0xDEADBEEFu);

	uint8_t out[4] = {0, 0, 0, 0};
	store_be32(out, 0x01020304u);
	REQUIRE(out[0] == 0x01);
	REQUIRE(out[1] == 0x02);
	REQUIRE(out[2] == 0x03);
	REQUIRE(out[3] == 0x04);

	SECTION("store потом load - тождество") {
		store_be32(out, 0x89ABCDEFu);
		REQUIRE(load_be32(out) == 0x89ABCDEFu);
	}
}

TEST_CASE("rotl32 - корректный циклический сдвиг", "[shacal1][rotl]") {
	REQUIRE(rotl32(0x00000001u, 0) == 0x00000001u);   // сдвиг на 0 не UB
	REQUIRE(rotl32(0x80000000u, 1) == 0x00000001u);   // старший бит уходит в младший
	REQUIRE(rotl32(0x12345678u, 32) == 0x12345678u);  // сдвиг на 32 == 0
	REQUIRE(rotl32(0x12345678u, 8) == 0x34567812u);
}

TEST_CASE("expand_key: 16 -> 80 слов и унаследованный квирк w[16]", "[shacal1][schedule]") {
	const Key key = test::fixed_key();
	const Schedule w = expand_key(key);

	SECTION("первые 16 слов совпадают с ключом") {
		for (std::size_t i = 0; i < kKeyWords; i++) {
			REQUIRE(w[i] == key[i]);
		}
	}

	SECTION("w[16] остаётся нулевым (сознательно сохранённый квирк)") {
		REQUIRE(w[16] == 0u);
	}

	SECTION("слова i>16 удовлетворяют рекуррентности SHA-1") {
		for (std::size_t i = 17; i < kRounds; i++) {
			const uint32_t expected =
				rotl32(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
			REQUIRE(w[i] == expected);
		}
	}
}

TEST_CASE("encrypt_block / decrypt_block - round-trip одного блока", "[shacal1][block]") {
	const Schedule schedule = expand_key(test::fixed_key());

	Block pt = {0x00010203u, 0x04050607u, 0x08090A0Bu, 0x0C0D0E0Fu, 0x10111213u};
	const Block ct = encrypt_block(pt, schedule, kDefaultRoundConsts);
	const Block back = decrypt_block(ct, schedule, kDefaultRoundConsts);

	REQUIRE(back == pt);
	REQUIRE(ct != pt);  // шифрование действительно меняет блок
}

TEST_CASE("Golden-вектор блочного преобразования (regression)", "[shacal1][vector]") {
	// Значение зафиксировано из текущей реализации; меняться не должно без
	// осознанного решения (иначе ломается совместимость с прежним выходом).
	const Schedule schedule = expand_key(test::fixed_key());
	const Block pt = {0x00010203u, 0x04050607u, 0x08090A0Bu, 0x0C0D0E0Fu, 0x10111213u};
	const Block ct = encrypt_block(pt, schedule, kDefaultRoundConsts);

	const Block expected = {0xD4B0A13Eu, 0xB74DD153u, 0x88D1C00Du, 0x8AC7AB1Fu, 0x8DD437FDu};
	REQUIRE(ct == expected);
}

TEST_CASE("Round-trip блока с пользовательскими раундовыми константами", "[shacal1][consts]") {
	const Schedule schedule = expand_key(test::fixed_key());
	const RoundConsts custom = {0x11111111u, 0x22222222u, 0x33333333u, 0x44444444u};

	const Block pt = {0xDEADBEEFu, 0xCAFEBABEu, 0x01234567u, 0x89ABCDEFu, 0x0BADF00Du};
	const Block ct = encrypt_block(pt, schedule, custom);
	REQUIRE(decrypt_block(ct, schedule, custom) == pt);

	SECTION("другой набор констант даёт другой шифртекст") {
		const Block ct_default = encrypt_block(pt, schedule, kDefaultRoundConsts);
		REQUIRE(ct != ct_default);
	}
}
