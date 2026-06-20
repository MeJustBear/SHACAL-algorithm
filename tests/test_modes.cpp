// Тесты режимов сцепления (ECB/CBC/CFB/OFB):
//  - round-trip для всех длин 0..2*BLOCK (в т.ч. невыровненных, включая 57 байт);
//  - golden-векторы (regression, зафиксированы из реализации);
//  - валидация дополнения: повреждённый шифртекст / неверные константы дают
//    PaddingError, а не std::length_error.
//
// Важно: имена TEST_CASE — только ASCII. Кириллица в имени теста ломает прогон
// под Windows: catch_discover_tests сохраняет имя в UTF-8, а ctest передаёт его
// обратно в фильтр в кодировке консоли (cp866/cp1251) — байты не совпадают,
// тест «не находится» и помечается как Failed. Поэтому описания — в комментариях.

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <stdexcept>
#include <vector>

#include "common.hpp"
#include "errors.hpp"
#include "modes/cbc.hpp"
#include "modes/cfb.hpp"
#include "modes/ecb.hpp"
#include "modes/ofb.hpp"

using namespace shacal;

namespace {

// Round-trip конкретного режима для одной длины.
void check_roundtrip(const std::string& mode, std::size_t len) {
	const Key key = test::fixed_key();
	const Block iv = test::fixed_iv();
	const RoundConsts& c = kDefaultRoundConsts;
	const std::vector<uint8_t> msg = test::incrementing(len);

	std::vector<uint8_t> ct, pt;
	if (mode == "ecb") {
		ct = modes::ecb_encrypt(msg, key, c);
		pt = modes::ecb_decrypt(ct, key, c);
	} else if (mode == "cbc") {
		ct = modes::cbc_encrypt(msg, key, iv, c);
		pt = modes::cbc_decrypt(ct, key, iv, c);
	} else if (mode == "cfb") {
		ct = modes::cfb_encrypt(msg, key, iv, c);
		pt = modes::cfb_decrypt(ct, key, iv, c);
	} else {
		ct = modes::ofb_encrypt(msg, key, iv, c);
		pt = modes::ofb_decrypt(ct, key, iv, c);
	}

	INFO("mode=" << mode << " len=" << len);
	REQUIRE(ct.size() % kBlockBytes == 0);  // шифртекст всегда выровнен
	REQUIRE(pt.size() == len);              // длина восстанавливается точно
	REQUIRE(pt == msg);                     // и содержимое тоже
}

}  // namespace

// Round-trip: перебор длин 0..2*BLOCK для каждого режима.
TEST_CASE("modes: round-trip over lengths 0..2*BLOCK for each mode", "[modes][roundtrip]") {
	for (const std::string mode : {"ecb", "cbc", "cfb", "ofb"}) {
		for (std::size_t len = 0; len <= 2 * kBlockBytes; len++) {
			check_roundtrip(mode, len);
		}
	}
}

// Регрессия: раньше самодельная схема дополнения теряла последний байт на части
// невыровненных длин (в частности 57 -> 56). PKCS#7 это закрывает.
TEST_CASE("modes: regression - unaligned 57-byte input is fully restored", "[modes][regression]") {
	for (const std::string mode : {"ecb", "cbc", "cfb", "ofb"}) {
		check_roundtrip(mode, 57);
	}
}

// Golden-векторы режимов (regression).
TEST_CASE("modes: golden vectors (regression)", "[modes][vectors]") {
	const Key key = test::fixed_key();
	const Block iv = test::fixed_iv();
	const RoundConsts& c = kDefaultRoundConsts;
	const std::vector<uint8_t> msg = test::incrementing(50);

	REQUIRE(test::to_hex(modes::ecb_encrypt(msg, key, c)) ==
		"d4b0a13eb74dd15388d1c00d8ac7ab1f8dd437fdf8847942699be438ea15b8fb"
		"84f1bb13cc9592bc4798a55717390f2822cd75eff0a279a307fad7ec");
	REQUIRE(test::to_hex(modes::cbc_encrypt(msg, key, iv, c)) ==
		"44f0a4a8806c6e8205bdbcf79d382ba8f00c57fdbd452401d5d91119d914e526"
		"d37f26e204dc89fac17c1284aa1893428f4181c0774ab7548c88db48");
	REQUIRE(test::to_hex(modes::cfb_encrypt(msg, key, iv, c)) ==
		"a064ecc038bfe8a8092dd528ae399363d683b08818c1e06d63e04e292f9399f1"
		"95ebd6765be04aee7bd16e087d3d134b0fd64e81f05378c9bedb4104");
	REQUIRE(test::to_hex(modes::ofb_encrypt(msg, key, iv, c)) ==
		"a064ecc038bfe8a8092dd528ae399363d683b0883da7094e6c45c89be5b8a667"
		"3461d658a3ad161c0d003c0c2b51e401ba534aface099fae5aca07b9");
}

// Шифртекст некратной блоку длины -> PaddingError.
TEST_CASE("modes: non-block-aligned ciphertext -> PaddingError", "[modes][validation]") {
	const Key key = test::fixed_key();
	const Block iv = test::fixed_iv();
	const RoundConsts& c = kDefaultRoundConsts;
	std::vector<uint8_t> bad(kBlockBytes + 3, 0x00);  // 23 байта - не кратно 20

	REQUIRE_THROWS_AS(modes::ecb_decrypt(bad, key, c), PaddingError);
	REQUIRE_THROWS_AS(modes::cbc_decrypt(bad, key, iv, c), PaddingError);
	REQUIRE_THROWS_AS(modes::cfb_decrypt(bad, key, iv, c), PaddingError);
	REQUIRE_THROWS_AS(modes::ofb_decrypt(bad, key, iv, c), PaddingError);
}

// Повреждённый шифртекст -> PaddingError, не length_error.
TEST_CASE("modes: corrupted ciphertext -> PaddingError (not length_error)", "[modes][validation]") {
	const Key key = test::fixed_key();
	const Block iv = test::fixed_iv();
	const RoundConsts& c = kDefaultRoundConsts;
	const std::vector<uint8_t> msg = test::incrementing(50);

	std::vector<uint8_t> ct = modes::cbc_encrypt(msg, key, iv, c);
	ct.back() ^= 0xFF;  // портим последний байт
	REQUIRE_THROWS_AS(modes::cbc_decrypt(ct, key, iv, c), PaddingError);
}

// Неверный набор раундовых констант -> PaddingError.
TEST_CASE("modes: wrong round constants -> PaddingError", "[modes][validation]") {
	const Key key = test::fixed_key();
	const Block iv = test::fixed_iv();
	const std::vector<uint8_t> msg = test::incrementing(50);
	const RoundConsts wrong = {0x11111111u, 0x22222222u, 0x33333333u, 0x44444444u};

	const std::vector<uint8_t> ct_ecb = modes::ecb_encrypt(msg, key, kDefaultRoundConsts);
	REQUIRE_THROWS_AS(modes::ecb_decrypt(ct_ecb, key, wrong), PaddingError);

	const std::vector<uint8_t> ct_cbc = modes::cbc_encrypt(msg, key, iv, kDefaultRoundConsts);
	REQUIRE_THROWS_AS(modes::cbc_decrypt(ct_cbc, key, iv, wrong), PaddingError);
}

// Гарантия из roadmap: на любом мусоре корректной длины unpad даёт либо валидный
// результат, либо PaddingError - но не «молчаливый» std::length_error.
TEST_CASE("modes: decrypt never throws std::length_error", "[modes][validation]") {
	const Key key = test::fixed_key();
	const Block iv = test::fixed_iv();
	const RoundConsts& c = kDefaultRoundConsts;

	for (std::size_t blocks = 1; blocks <= 4; blocks++) {
		std::vector<uint8_t> garbage(blocks * kBlockBytes);
		for (std::size_t i = 0; i < garbage.size(); i++) {
			garbage[i] = static_cast<uint8_t>(i * 37 + 13);
		}
		try {
			modes::cbc_decrypt(garbage, key, iv, c);
		} catch (const PaddingError&) {
			// допустимо
		} catch (const std::length_error&) {
			FAIL("cbc_decrypt threw std::length_error on garbage");
		}
	}
}
