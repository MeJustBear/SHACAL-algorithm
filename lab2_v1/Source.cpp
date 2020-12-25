#include"ServiceHeader.h"
#include"Header.h"
#include"HeaderForBmp.h"
#include <numeric>

using namespace std;


vector<uint32_t>& encrypt_block(vector<uint32_t>& block, vector<uint32_t>& key, const uint32_t* consts) {
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

		A[1] = static_cast<uint32_t> (static_cast<unsigned long long>(key[i] + cycle_move_left(A[0], 5) + function(B[0], C[0], D[0], i) + E[0] + consts[i / 20]) % static_cast<unsigned long long>(pow(2, 32)));
		B[1] = A[0];
		C[1] = cycle_move_left(B[0], 30);
		D[1] = C[0];
		E[1] = D[0];
	}
	vector<uint32_t>* sipher = new vector<uint32_t>;

	sipher->push_back(A[1]);
	sipher->push_back(B[1]);
	sipher->push_back(C[1]);
	sipher->push_back(D[1]);
	sipher->push_back(E[1]);

	return *sipher;
}

vector<uint32_t>& decrypt_block(vector<uint32_t>& block, vector<uint32_t>& key, const uint32_t * consts) {
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
		E[0] = static_cast<uint32_t>(static_cast<unsigned long long>(~key[FULL_KEY_LENGHT - 1 - i] + ~cycle_move_left(B[1], 5) +
			~function(B[0], D[1], E[1], FULL_KEY_LENGHT - 1 - i) + A[1] + ~consts[(FULL_KEY_LENGHT - 1 - i) / 20] + 4) %
			static_cast<unsigned long long>(pow(2, 32)));
	}
	vector<uint32_t>* opentext = new vector<uint32_t>;

	opentext->push_back(A[0]);
	opentext->push_back(B[0]);
	opentext->push_back(C[0]);
	opentext->push_back(D[0]);
	opentext->push_back(E[0]);

	return *opentext;
}

vector<uint32_t>& key_generate() {
	unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
	std::default_random_engine generator;
	std::uniform_int_distribution<uint32_t> distribution(seed);
	vector<uint32_t>* key = new vector<uint32_t>(KEY_LENGHT);
	uint8_t i = 0;
	while (i < KEY_LENGHT) {
		(*key)[i] = distribution(generator);
		i++;
	}
	i = 0;
	return *key;
}

vector<uint32_t>& key_addiction(vector<uint32_t>& key) {
	vector<uint32_t>* addictedKey = new vector<uint32_t>(FULL_KEY_LENGHT);

	for (int8_t i = 0; i < FULL_KEY_LENGHT; i++) {
		if (i < 16) {
			(*addictedKey)[i] = key[i];
		}
		if (i > 16) {
			(*addictedKey)[i] = cycle_move_left((*addictedKey)[i - 3] ^ (*addictedKey)[i - 8] ^ (*addictedKey)[i - 14] ^ (*addictedKey)[i - 16], 1);
		}
	}
	return *addictedKey;
}

vector<uint32_t>& vector_init() {
	unsigned seed = std::chrono::steady_clock::now().time_since_epoch().count();
	std::default_random_engine generator;
	std::uniform_int_distribution<uint32_t> distribution(seed);
	vector<uint32_t>* key = new vector<uint32_t>(BLOCKS_SIZE);
	uint8_t i = 0;
	while (i < BLOCKS_SIZE) {
		(*key)[i] = distribution(generator);
		i++;
	}
	i = 0;
	return *key;
}

vector<uint32_t>& create_special_block() {
	vector<uint32_t>* dest = new vector<uint32_t>;
	dest->resize(BLOCKS_SIZE);

	(*dest)[BLOCKS_SIZE - 1] = ZERO_BLOCK;
	return *dest;
}

vector<uint8_t>& finalise_res(vector<uint32_t>& result) {
	uint8_t count = result[result.size() - 1];
	count++;
	result.resize(result.size() - (count / BLOCK_SIZE));
	uint8_t tmp = count % BLOCK_SIZE;
	size_t size = result.size();
	vector<uint8_t>* ans = new vector<uint8_t>;
	if (tmp != 0) {
		switch (tmp) {
		case 1:
			ans->push_back(result[size - 1] >> MOVE_RIGHT_F);
		case 2:
			ans->push_back((result[size - 1] << MOVE_RIGHT_T) >> MOVE_RIGHT_F);
		case 3:
			ans->push_back((result[size - 1] << MOVE_RIGHT_S) >> MOVE_RIGHT_F);
			break;
		}
	}
	result.resize(size);
	return *ans;
}

uint32_t* read_consts() {
	ifstream fin("consts.txt");
	uint32_t* consts = new uint32_t[CONSTS_SIZE];
	uint8_t i = 0;
	while (!fin.eof()) {
		fin >> hex >> consts[i];
		i++;
	}
	fin.close();
	return consts;
}

uint32_t cycle_move_left(uint32_t param, int8_t len) {
	uint32_t paramR = (uint32_t)param;
	return((paramR << len) | (paramR >> (INT_SIZE - len)));
}

uint32_t function(uint32_t x, uint32_t y, uint32_t z, uint8_t i) {
	if (i < 20) {
		return((x & y) | (~x & z));
	}
	if ((i >= 20 && i < 40) || (i >= 60 && i < 80)) {
		return(x^y^z);
	}
	if (i >= 40 && i < 60) {
		return((x^y) | (x^z) | (y^z));
	}
}

bool read_block(ifstream& fin, vector<uint32_t>& dest, size_t maxsize, size_t* currentPointer, bool* param, bool* paramd) {
	dest.clear();
	dest.resize(BLOCKS_SIZE);
	unsigned char c = 0;
	uint8_t current_int_count = 0;
	uint8_t current_block_count = 0;
	bool b = !fin.eof();
	while ((*currentPointer) != maxsize && current_block_count < BLOCKS_SIZE) {
		if ((*currentPointer) == maxsize) {
			paramd = false;
			break;
		}
		if ((*currentPointer) == 127050) {
			bool b = true;
		}
		(*currentPointer)++;
		fin.read((char*)&c, sizeof(char));
		if (current_int_count == 0) {
			uint32_t qword = c;
			qword = qword << MOVE_RIGHT_F;
			dest[current_block_count] = dest[current_block_count] | qword;
		}
		if (current_int_count == 1) {
			uint32_t qword = c;
			qword = qword << MOVE_RIGHT_S;
			dest[current_block_count] = dest[current_block_count] | qword;
			goto label;
		}
		if (current_int_count == 2) {
			uint32_t qword = c;
			qword = qword << MOVE_RIGHT_T;
			dest[current_block_count] = dest[current_block_count] | qword;
			goto label;
		}
		if (current_int_count == 3) {
			uint32_t qword = c;
			dest[current_block_count] = dest[current_block_count] | qword;
			current_int_count = 0;
			current_block_count++;
			goto nextlabel;
		}
	label:
		current_int_count++;
		b = false;
		continue;
	nextlabel:
		b = true;
		continue;
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

void write_result(vector<uint32_t>& result, string filename) {
	ofstream fout(filename, ios::binary);
	for (size_t i = 0; i < result.size(); i++) {
		char c = result[i] >> MOVE_RIGHT_F;
		fout.write((char*)&c, 1);
		c = (result[i] << MOVE_RIGHT_T) >> MOVE_RIGHT_F;
		fout.write((char*)&c, 1);
		c = (result[i] << MOVE_RIGHT_S) >> MOVE_RIGHT_F;
		fout.write((char*)&c, 1);
		c = (result[i] << MOVE_RIGHT_F) >> MOVE_RIGHT_F;
		fout.write((char*)&c, 1);
	}
	fout.close();
}

void write_result8(vector<uint8_t>& res, string filename) {
	ofstream fout(filename, ios::app | ios::binary);
	if (!res.empty())
		for (uint8_t i = 0; i < res.size(); i++) {
			fout.write((char*)&res[i], 1);
		}
	fout.close();
}

void read_init_vec(vector<uint32_t>& init, string filename) {
	std::ifstream input(filename, std::ios::binary);
	std::vector<unsigned char> text(
		(std::istreambuf_iterator<char>(input)),
		(std::istreambuf_iterator<char>()));
	input.close();
	size_t j = 0;

	for (size_t i = 0; i < text.size() && j < BLOCKS_SIZE; i++, j++) {
		char str[8];
		uint8_t cur = 0;
		while (text[i] != ' ' && i < text.size()) {
			str[cur] = text[i];
			i++;
			cur++;
			if (i == text.size()) break;
		}
		char* pEnd;
		init[j] = static_cast<uint32_t>(strtoul(str, &pEnd, 16));
	}
	input.close();
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



