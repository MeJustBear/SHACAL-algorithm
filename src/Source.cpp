#include"ServiceHeader.h"
#include"Header.h"
#include <array>
#include <cassert>
#include <cstdint>
#include <limits>
#include <sstream>
#include <stdexcept>
#include <string>
#include<cmath>

using namespace std;

namespace {

uint32_t add_mod32(uint64_t a, uint64_t b, uint64_t c, uint64_t d, uint64_t e, uint64_t f) {
	return static_cast<uint32_t>(a + b + c + d + e + f);
}

vector<uint32_t> fill_random_vector(size_t count) {
	vector<uint32_t> values(count);
	std::random_device rd;
	std::default_random_engine generator(rd());
	std::uniform_int_distribution<uint32_t> distribution(
		std::numeric_limits<uint32_t>::min(),
		std::numeric_limits<uint32_t>::max());
	for (size_t i = 0; i < count; i++) {
		values[i] = distribution(generator);
	}
	return values;
}

}  // namespace

vector<uint32_t> encrypt_block(vector<uint32_t>& block, vector<uint32_t>& key, const uint32_t* consts) {
	uint32_t A[2];
	A[0] = block[0];
	A[1] = A[0];
	uint32_t B[2];
	B[0] = block[1];
	B[1] = B[0];
	uint32_t C[2];
	C[0] = block[2];
	C[1] = C[0];
	uint32_t D[2];
	D[0] = block[3];
	D[1] = D[0];
	uint32_t E[2];
	E[0] = block[4];
	E[1] = E[0];

	for (uint8_t i = 0; i < FULL_KEY_LENGHT; i++) {
		A[0] = A[1];
		B[0] = B[1];
		C[0] = C[1];
		D[0] = D[1];
		E[0] = E[1];

		A[1] = add_mod32(key[i], cycle_move_left(A[0], 5), function(B[0], C[0], D[0], i), E[0],
			consts[i / 20], 0);
		B[1] = A[0];
		C[1] = cycle_move_left(B[0], 30);
		D[1] = C[0];
		E[1] = D[0];
	}

	return vector<uint32_t>{A[1], B[1], C[1], D[1], E[1]};
}

vector<uint32_t> decrypt_block(vector<uint32_t>& block, vector<uint32_t>& key, const uint32_t* consts) {
	uint32_t A[2];
	A[1] = block[0];
	A[0] = A[1];
	uint32_t B[2];
	B[1] = block[1];
	B[0] = B[1];
	uint32_t C[2];
	C[1] = block[2];
	C[0] = C[1];
	uint32_t D[2];
	D[1] = block[3];
	D[0] = D[1];
	uint32_t E[2];
	E[1] = block[4];
	E[0] = E[1];

	for (uint8_t i = 0; i < FULL_KEY_LENGHT; i++) {
		A[1] = A[0];
		B[1] = B[0];
		C[1] = C[0];
		D[1] = D[0];
		E[1] = E[0];

		A[0] = B[1];
		B[0] = cycle_move_left(C[1], 2);
		C[0] = D[1];
		D[0] = E[1];
		const uint8_t round = FULL_KEY_LENGHT - 1 - i;
		E[0] = add_mod32(~key[round], ~cycle_move_left(B[1], 5),
			~function(B[0], D[1], E[1], round), A[1], ~consts[round / 20], 4);
	}

	return vector<uint32_t>{A[0], B[0], C[0], D[0], E[0]};
}

vector<uint32_t> key_generate() {
	return fill_random_vector(KEY_LENGHT);
}

vector<uint32_t> key_addiction(vector<uint32_t>& key) {
	vector<uint32_t> addictedKey(FULL_KEY_LENGHT);

	for (int8_t i = 0; i < FULL_KEY_LENGHT; i++) {
		if (i < 16) {
			addictedKey[i] = key[i];
		}
		if (i > 16) {
			addictedKey[i] = cycle_move_left(
				addictedKey[i - 3] ^ addictedKey[i - 8] ^ addictedKey[i - 14] ^ addictedKey[i - 16], 1);
		}
	}
	return addictedKey;
}

vector<uint32_t> vector_init() {
	return fill_random_vector(BLOCKS_SIZE);
}

vector<uint32_t> create_special_block() {
	vector<uint32_t> dest(BLOCKS_SIZE);
	dest[BLOCKS_SIZE - 1] = ZERO_BLOCK;
	return dest;
}

vector<uint8_t> finalise_res(vector<uint32_t>& result) {
	uint8_t count = result[result.size() - 1];
	count++;
	result.resize(result.size() - (count / BLOCK_SIZE));
	uint8_t tmp = count % BLOCK_SIZE;
	size_t size = result.size();
	vector<uint8_t> ans;
	if (tmp != 0) {
		switch (tmp) {
		case 1:
			ans.push_back(result[size - 1] >> MOVE_RIGHT_F);
		case 2:
			ans.push_back((result[size - 1] << MOVE_RIGHT_T) >> MOVE_RIGHT_F);
		case 3:
			ans.push_back((result[size - 1] << MOVE_RIGHT_S) >> MOVE_RIGHT_F);
			break;
		}
	}
	result.resize(size);
	return ans;
}

vector<uint32_t> read_hex_values(const string& filename, size_t count) {
	ifstream fin(filename);
	if (!fin.is_open()) {
		throw runtime_error("cannot open file: " + filename);
	}
	vector<uint32_t> values;
	values.reserve(count);
	uint32_t value = 0;
	while (values.size() < count && (fin >> hex >> value)) {
		values.push_back(value);
	}
	if (values.size() < count) {
		throw runtime_error("not enough hex values in file: " + filename);
	}
	return values;
}

void write_hex_values(const string& filename, const vector<uint32_t>& values) {
	ofstream fout(filename);
	if (!fout.is_open()) {
		throw runtime_error("cannot open file for writing: " + filename);
	}
	for (size_t i = 0; i < values.size(); i++) {
		fout << hex << values[i];
		if (i + 1 < values.size()) {
			fout << ' ';
		}
	}
}

array<uint32_t, 4> parse_round_consts(const string& csv) {
	array<uint32_t, 4> consts{};
	stringstream ss(csv);
	string token;
	size_t i = 0;
	while (getline(ss, token, ',') && i < consts.size()) {
		consts[i++] = static_cast<uint32_t>(stoul(token, nullptr, 16));
	}
	if (i != consts.size()) {
		throw runtime_error("--round-consts expects 4 comma-separated hex values");
	}
	return consts;
}

array<uint32_t, 4> read_round_consts_file(const string& filename) {
	vector<uint32_t> values = read_hex_values(filename, 4);
	return {values[0], values[1], values[2], values[3]};
}

uint32_t cycle_move_left(uint32_t param, int8_t len) {
	uint32_t paramR = (uint32_t)param;
	return((paramR << len) | (paramR >> (INT_SIZE - len)));
}

uint32_t function(uint32_t x, uint32_t y, uint32_t z, uint8_t i) {
	assert(i < FULL_KEY_LENGHT);
	if (i < 20) {
		return((x & y) | (~x & z));
	}
	if ((i >= 20 && i < 40) || (i >= 60 && i < 80)) {
		return(x ^ y ^ z);
	}
	if (i >= 40 && i < 60) {
		return((x ^ y) | (x ^ z) | (y ^ z));
	}
	return 0;
}

bool read_block(ifstream& fin, vector<uint32_t>& dest, size_t maxsize, size_t* currentPointer, bool* param, bool* paramd) {
	dest.clear();
	dest.resize(BLOCKS_SIZE);
	unsigned char c = 0;
	uint8_t current_int_count = 0;
	uint8_t current_block_count = 0;
	bool b = !fin.eof();
	const uint32_t shifts[] = {MOVE_RIGHT_F, MOVE_RIGHT_S, MOVE_RIGHT_T, 0};

	while ((*currentPointer) != maxsize && current_block_count < BLOCKS_SIZE) {
		if ((*currentPointer) == maxsize) {
			*paramd = false;
			break;
		}
		(*currentPointer)++;
		fin.read((char*)&c, sizeof(char));

		const uint32_t qword = static_cast<uint32_t>(c) << shifts[current_int_count];
		dest[current_block_count] |= qword;

		if (current_int_count == 3) {
			current_int_count = 0;
			current_block_count++;
			b = true;
		} else {
			current_int_count++;
			b = false;
		}
	}
	if (!b) {
		current_int_count--;
	}
	if (current_block_count == BLOCKS_SIZE && current_int_count == 0 && (*currentPointer) == maxsize) {
		(*param) = false;
		return true;
	}
	else if (current_block_count <= BLOCKS_SIZE - 1 && (*currentPointer) == maxsize) {
		dest[BLOCKS_SIZE - 1] = dest[BLOCKS_SIZE - 1] | ((BLOCK_SIZE - 1 - current_int_count) + (BLOCKS_SIZE - 1 - current_block_count) * 4);
		(*param) = false;
		return false;
	}
	return false;
}

void dispersion(std::vector<uint32_t>& res){
	size_t size = res.size();
	size_t zeros = 0;
	size_t ones = 0;
	for (size_t i = 0; i < size; i++) {
		uint32_t tmp = res[i];
		uint8_t count = 0;
		while (tmp) {
			if (tmp & 1) {
				ones++;
			}
			else {
				zeros++;
			}
			tmp = tmp >> 1;
			count++;
			if (count == 32 && tmp == 0) {
				zeros++;
			}
		}
	}
	size = size * 32;
	cout << "\ndisp of 0: \n" << static_cast<double>(zeros) / static_cast<double>(size);
	cout << "\ndisp of 1: \n" << static_cast<double>(ones) / static_cast<double>(size);

}

void correlation(std::vector<uint32_t>& in, std::vector<uint32_t>& out){
	double U = 0;
	double u = 0;
	for (size_t i = 0; i < in.size(); i++) {
		uint32_t tmp = in[i];
		U += pow(tmp >> MOVE_RIGHT_F,2);
		U += pow((tmp << MOVE_RIGHT_T) >> MOVE_RIGHT_F, 2);
		U += pow((tmp << MOVE_RIGHT_S) >> MOVE_RIGHT_F, 2);
		U += pow((tmp << MOVE_RIGHT_F) >> MOVE_RIGHT_F, 2);

	}

	for (size_t i = 0; i < out.size(); i++) {
		uint32_t tmp = out[i];
		u += pow(tmp >> MOVE_RIGHT_F, 2);
		u += pow((tmp << MOVE_RIGHT_T) >> MOVE_RIGHT_F, 2);
		u += pow((tmp << MOVE_RIGHT_S) >> MOVE_RIGHT_F, 2);
		u += pow((tmp << MOVE_RIGHT_F) >> MOVE_RIGHT_F, 2);
		//u += static_cast<double>(out[i]) * static_cast<double>(out[i]);
	}
	u = sqrt(u);
	U = sqrt(U);
	
	double sum = 0;
	for (size_t i = 0; i < in.size(); i++) {
		uint32_t tmp = in[i] & out[i];
		sum += pow(tmp >> MOVE_RIGHT_F, 2);
		sum += pow((tmp << MOVE_RIGHT_T) >> MOVE_RIGHT_F, 2);
		sum += pow((tmp << MOVE_RIGHT_S) >> MOVE_RIGHT_F, 2);
		sum += pow((tmp << MOVE_RIGHT_F) >> MOVE_RIGHT_F, 2);
	}

	sum = sum / (u * U);

	cout << "\ncorrelation is: \n" << sum << endl;
}


