#include "streams.hpp"

#include <fstream>
#include <iomanip>
#include <ios>
#include <stdexcept>

namespace shacal::io {

std::vector<uint8_t> read_file(const std::string& path) {
	std::ifstream fin(path, std::ios::binary);
	if (!fin.is_open()) {
		throw std::runtime_error("cannot open file: " + path);
	}
	return std::vector<uint8_t>(std::istreambuf_iterator<char>(fin),
		std::istreambuf_iterator<char>());
}

void write_file(const std::string& path, const std::vector<uint8_t>& data) {
	std::ofstream fout(path, std::ios::binary);
	if (!fout.is_open()) {
		throw std::runtime_error("cannot open file for writing: " + path);
	}
	if (!data.empty()) {
		fout.write(reinterpret_cast<const char*>(data.data()),
			static_cast<std::streamsize>(data.size()));
	}
}

std::vector<uint32_t> read_hex_values(const std::string& path, std::size_t count) {
	std::ifstream fin(path);
	if (!fin.is_open()) {
		throw std::runtime_error("cannot open file: " + path);
	}
	std::vector<uint32_t> values;
	values.reserve(count);
	uint32_t value = 0;
	while (values.size() < count && (fin >> std::hex >> value)) {
		values.push_back(value);
	}
	if (values.size() < count) {
		throw std::runtime_error("not enough hex values in file: " + path);
	}
	return values;
}

void write_hex_values(const std::string& path, const std::vector<uint32_t>& values) {
	std::ofstream fout(path);
	if (!fout.is_open()) {
		throw std::runtime_error("cannot open file for writing: " + path);
	}
	for (std::size_t i = 0; i < values.size(); i++) {
		fout << std::hex << values[i];
		if (i + 1 < values.size()) {
			fout << ' ';
		}
	}
}

}  // namespace shacal::io
