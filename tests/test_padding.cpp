// Тесты дополнения PKCS#7: корректность pad/unpad для любых длин, валидация
// при снятии (закрытие дефектов старой самодельной схемы из roadmap, Этап 3).

#include <catch2/catch_test_macros.hpp>

#include <cstdint>
#include <vector>

#include "common.hpp"
#include "errors.hpp"
#include "padding.hpp"

using namespace shacal;

TEST_CASE("padding: pkcs7_pad rounds length up to a block multiple", "[padding]") {
	for (std::size_t len = 0; len <= 2 * kBlockBytes; len++) {
		std::vector<uint8_t> data = test::incrementing(len);
		pkcs7_pad(data);

		REQUIRE(data.size() % kBlockBytes == 0);
		REQUIRE(data.size() > len);  // всегда добавляется 1..block байт

		const std::size_t pad = data.back();
		REQUIRE(pad >= 1);
		REQUIRE(pad <= kBlockBytes);
		// Последние pad байт равны значению pad.
		for (std::size_t i = 0; i < pad; i++) {
			REQUIRE(data[data.size() - 1 - i] == pad);
		}
	}
}

TEST_CASE("padding: pkcs7_pad + pkcs7_unpad is identity for any length", "[padding]") {
	for (std::size_t len = 0; len <= 2 * kBlockBytes; len++) {
		std::vector<uint8_t> data = test::incrementing(len);
		const std::vector<uint8_t> original = data;

		pkcs7_pad(data);
		pkcs7_unpad(data);
		REQUIRE(data == original);
	}
}

TEST_CASE("padding: block-aligned length is padded with a whole block", "[padding]") {
	std::vector<uint8_t> data = test::incrementing(kBlockBytes);
	pkcs7_pad(data);
	REQUIRE(data.size() == 2 * kBlockBytes);
	REQUIRE(data.back() == kBlockBytes);  // добавлен полный блок маркеров
}

TEST_CASE("padding: pkcs7_unpad rejects invalid padding", "[padding][errors]") {
	SECTION("empty buffer") {
		std::vector<uint8_t> data;
		REQUIRE_THROWS_AS(pkcs7_unpad(data), PaddingError);
	}

	SECTION("length not a multiple of the block") {
		std::vector<uint8_t> data(kBlockBytes + 1, 0x01);
		REQUIRE_THROWS_AS(pkcs7_unpad(data), PaddingError);
	}

	SECTION("marker == 0") {
		std::vector<uint8_t> data(kBlockBytes, 0x00);
		REQUIRE_THROWS_AS(pkcs7_unpad(data), PaddingError);
	}

	SECTION("marker larger than the block size") {
		std::vector<uint8_t> data(kBlockBytes, 0x00);
		data.back() = static_cast<uint8_t>(kBlockBytes + 1);
		REQUIRE_THROWS_AS(pkcs7_unpad(data), PaddingError);
	}

	SECTION("inconsistent padding bytes") {
		std::vector<uint8_t> data(kBlockBytes, 0x00);
		data.back() = 0x03;  // заявлено 3 байта дополнения,
		// но data[size-2] и data[size-3] == 0, а не 3
		REQUIRE_THROWS_AS(pkcs7_unpad(data), PaddingError);
	}
}

TEST_CASE("padding: pkcs7_pad rejects an invalid block size", "[padding][errors]") {
	std::vector<uint8_t> data = test::incrementing(4);
	REQUIRE_THROWS_AS(pkcs7_pad(data, 0), InvalidArgument);
	REQUIRE_THROWS_AS(pkcs7_pad(data, 256), InvalidArgument);
}
