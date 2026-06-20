#include "stats.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <iostream>

#include "../core/shacal1.hpp"

namespace shacal::cli {

namespace {

// Сумма квадратов четырёх байт слова - как в исходной метрике корреляции.
double sum_byte_squares(uint32_t word) {
	const double b0 = static_cast<double>((word >> 24) & 0xFFu);
	const double b1 = static_cast<double>((word >> 16) & 0xFFu);
	const double b2 = static_cast<double>((word >> 8) & 0xFFu);
	const double b3 = static_cast<double>(word & 0xFFu);
	return b0 * b0 + b1 * b1 + b2 * b2 + b3 * b3;
}

unsigned popcount32(uint32_t value) {
	unsigned count = 0;
	while (value) {
		count += value & 1u;
		value >>= 1;
	}
	return count;
}

}  // namespace

std::vector<uint32_t> bytes_to_words_be(const std::vector<uint8_t>& bytes) {
	std::vector<uint32_t> words;
	words.reserve(bytes.size() / 4);
	for (std::size_t off = 0; off + 4 <= bytes.size(); off += 4) {
		words.push_back(load_be32(bytes.data() + off));
	}
	return words;
}

void dispersion(const std::vector<uint32_t>& data) {
	std::size_t ones = 0;
	for (uint32_t word : data) {
		ones += popcount32(word);
	}
	const std::size_t total = data.size() * 32;
	const std::size_t zeros = total - ones;
	const double denom = total == 0 ? 1.0 : static_cast<double>(total);
	std::cout << "\ndisp of 0: \n" << static_cast<double>(zeros) / denom;
	std::cout << "\ndisp of 1: \n" << static_cast<double>(ones) / denom << '\n';
}

void correlation(const std::vector<uint32_t>& in, const std::vector<uint32_t>& out) {
	double sum_in = 0.0;
	for (uint32_t word : in) {
		sum_in += sum_byte_squares(word);
	}
	double sum_out = 0.0;
	for (uint32_t word : out) {
		sum_out += sum_byte_squares(word);
	}
	const double norm = std::sqrt(sum_in) * std::sqrt(sum_out);

	double cross = 0.0;
	const std::size_t n = std::min(in.size(), out.size());
	for (std::size_t i = 0; i < n; i++) {
		cross += sum_byte_squares(in[i] & out[i]);
	}

	const double result = norm == 0.0 ? 0.0 : cross / norm;
	std::cout << "\ncorrelation is: \n" << result << '\n';
}

}  // namespace shacal::cli
