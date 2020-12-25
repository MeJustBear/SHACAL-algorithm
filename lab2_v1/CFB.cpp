#include"ServiceHeader.h"
#include"Header.h"
#include"HeaderForBmp.h"
#include <algorithm>
#include <functional>

using namespace std;

namespace cfb {

	void decrypt(string decryptFileName, vector<uint32_t>& key, string outputFilename, string vectorFilename) {
		vector<uint32_t>* init = new vector<uint32_t>(BLOCKS_SIZE);
		read_init_vec((*init), vectorFilename);

		ifstream fin(decryptFileName, ios::binary);
		fin.seekg(0, ios::end);
		size_t maxsize = fin.tellg();
		size_t currentPointer = 0;
		fin.seekg(0, ios::beg);
		bool param = true;
		bool paramd = true;

		vector<uint32_t>& addictedKey = key_addiction(key);

		uint32_t* const consts = read_consts();

		vector<uint32_t> block(BLOCKS_SIZE);

		vector<uint32_t> result;
		result.reserve(1000);

		bool label = true;

		while (!fin.eof()) {
			label = !read_block(fin, block, maxsize, &currentPointer, &param, &paramd);

			if (!paramd) break;
			vector<uint32_t>& siphrotext = encrypt_block((*init), addictedKey, consts);

			transform(block.begin(), block.end(), siphrotext.begin(), siphrotext.begin(), bit_xor<uint32_t>());
			delete init;
			init = new vector<uint32_t>(block);

			result.push_back(siphrotext[0]);
			result.push_back(siphrotext[1]);
			result.push_back(siphrotext[2]);
			result.push_back(siphrotext[3]);
			result.push_back(siphrotext[4]);

			delete &siphrotext;

			if (currentPointer == maxsize) break;

		}

		result.shrink_to_fit();

		fin.close();

		vector<uint8_t>& tmp = finalise_res(result);

		write_result(result, outputFilename);
		write_result8(tmp, outputFilename);

		delete &addictedKey;
		delete consts;
	}

	void encrypt(string destinationFileName, string keyFilename, string outputFilename, string vectorFilename) {
		ifstream fin(destinationFileName, ios::binary);
		fin.seekg(0, ios::end);
		size_t maxsize = fin.tellg();
		size_t currentPointer = 0;
		fin.seekg(0, ios::beg);
		bool param = true;
		bool paramd = true;

		vector<uint32_t>& key = key_generate();
		vector<uint32_t>& addictedKey = key_addiction(key);

		ofstream fout(keyFilename);
		cout << "your key is:\n";
		for (int8_t i = 0; i < KEY_LENGHT; i++) {
			fout << hex << key[i] << ' ';
			cout << hex << key[i] << ' ';
		}
		fout.close();
		fout.clear();
		cout << endl;
		key.clear();

		vector<uint32_t>& initVec = vector_init();
		fout.open(vectorFilename);
		cout << "your vector is:\n";
		for (int8_t i = 0; i < BLOCKS_SIZE; i++) {
			fout << hex << initVec[i] << ' ';
			cout << hex << initVec[i] << ' ';
		}
		fout.close();
		fout.clear();
		cout << endl;

		uint32_t* const consts = read_consts();
		vector<uint32_t> block(BLOCKS_SIZE);

		vector<uint32_t> result;
		result.reserve(1000);

		vector<uint32_t> input;
		input.reserve(1000);

		bool labe = true;
		while (labe) {
			labe = !read_block(fin, block, maxsize, &currentPointer, &param, &paramd);

			input.push_back(block[0]);
			input.push_back(block[1]);
			input.push_back(block[2]);
			input.push_back(block[3]);
			input.push_back(block[4]);

			vector<uint32_t>& siphrotext = encrypt_block(initVec, addictedKey, consts);
	
			
			transform(block.begin(), block.end(), siphrotext.begin(), siphrotext.begin(), bit_xor<uint32_t>());
			initVec.clear();
			initVec = siphrotext;

			result.push_back(siphrotext[0]);
			result.push_back(siphrotext[1]);
			result.push_back(siphrotext[2]);
			result.push_back(siphrotext[3]);
			result.push_back(siphrotext[4]);

			delete &siphrotext;

			if (!labe) {
				block = create_special_block();
				
				input.push_back(block[0]);
				input.push_back(block[1]);
				input.push_back(block[2]);
				input.push_back(block[3]);
				input.push_back(block[4]);

				siphrotext = encrypt_block(initVec, addictedKey, consts);
				transform(block.begin(), block.end(), siphrotext.begin(), siphrotext.begin(), bit_xor<uint32_t>());
				result.push_back(siphrotext[0]);
				result.push_back(siphrotext[1]);
				result.push_back(siphrotext[2]);
				result.push_back(siphrotext[3]);
				result.push_back(siphrotext[4]);
				delete &siphrotext;
				break;
			}
			if (fin.eof()) {
				break;
			}
			if (!param) break;
		}

		delete &initVec;

		result.shrink_to_fit();

		write_result(result, outputFilename);

		correlation(input, result);
		dispersion(result);

		delete &addictedKey;
		delete consts;
		fin.close();
		fin.clear();
	}

	void decrypt(ifstream& fin, vector<uint32_t>& key, string outFilename, string vectorFilename) {
		size_t pos = 54;
		fin.seekg(0, ios::beg);
		BMPFILEHEADER fh = readFH(fin);
		BMPINFOHEADER ih = readIH(fin);
		fin.seekg(0, ios::end);
		size_t maxsize = fin.tellg();
		size_t currentPointer = pos;
		fin.seekg(pos, ios::beg);
		bool param = true;
		bool paramd = true;

		vector<uint32_t>* init = new vector<uint32_t>(BLOCKS_SIZE);
		read_init_vec((*init), vectorFilename);

		vector<uint32_t>& addictedKey = key_addiction(key);

		uint32_t* const consts = read_consts();

		vector<uint32_t> block(BLOCKS_SIZE);

		vector<uint32_t> result;
		result.reserve(1000);

		bool label = true;

		while (currentPointer <= maxsize) {
			label = !read_block(fin, block, maxsize, &currentPointer, &param, &paramd);

			if (!paramd) break;
			vector<uint32_t>& siphrotext = encrypt_block((*init), addictedKey, consts);

			transform(block.begin(), block.end(), siphrotext.begin(), siphrotext.begin(), bit_xor<uint32_t>());
			delete init;
			init = new vector<uint32_t>(block);

			result.push_back(siphrotext[0]);
			result.push_back(siphrotext[1]);
			result.push_back(siphrotext[2]);
			result.push_back(siphrotext[3]);
			result.push_back(siphrotext[4]);
			
			delete &siphrotext;

			if (currentPointer == maxsize) break;
		}

		result.shrink_to_fit();


		fin.close();
		delete &addictedKey;
		delete consts;

		vector<uint8_t>& tmp = finalise_res(result);

		ofstream foutp = Open_File_Write(outFilename);
		writeFH(foutp, fh);
		writeIH(foutp, ih);

		for (size_t i = 0; i < result.size(); i++) {
			char c = result[i] >> 24;
			foutp.write((char*)&c, 1);
			c = (result[i] << 8) >> 24;
			foutp.write((char*)&c, 1);
			c = (result[i] << 16) >> 24;
			foutp.write((char*)&c, 1);
			c = (result[i] << 24) >> 24;
			foutp.write((char*)&c, 1);
		}

		if (!tmp.empty())
			for (uint8_t i = 0; i < tmp.size(); i++) {
				foutp.write((char*)&result[i], 1);
			}
		foutp.close();
	}

	void encrypt(ifstream& fin, string keyFilename, string outFilename, string vectorFilename) {
		size_t pos = fin.tellg();
		fin.seekg(0, ios::beg);
		BMPFILEHEADER fh = readFH(fin);
		BMPINFOHEADER ih = readIH(fin);
		fin.seekg(0, ios::end);
		size_t maxsize = fin.tellg();
		size_t currentPointer = pos;
		fin.seekg(pos, ios::beg);
		bool param = true;
		bool paramd = true;

		vector<uint32_t>& key = key_generate();
		vector<uint32_t>& addictedKey = key_addiction(key);

		ofstream fout(keyFilename);
		cout << "your key is:\n";
		for (int8_t i = 0; i < KEY_LENGHT; i++) {
			fout << hex << key[i] << ' ';
			cout << hex << key[i] << ' ';
		}
		fout.close();
		fout.clear();
		cout << endl;
		key.clear();

		vector<uint32_t>& initVec = vector_init();
		fout.open(vectorFilename);
		cout << "your vector is:\n";
		for (int8_t i = 0; i < BLOCKS_SIZE; i++) {
			fout << hex << initVec[i] << ' ';
			cout << hex << initVec[i] << ' ';
		}
		fout.close();
		fout.clear();
		cout << endl;

		uint32_t* const consts = read_consts();
		vector<uint32_t> block(BLOCKS_SIZE);

		vector<uint32_t> result;
		result.reserve(1000);

		vector<uint32_t> input;
		input.reserve(1000);

		bool labe = true;
		while (labe) {

			labe = !read_block(fin, block, maxsize, &currentPointer, &param, &paramd);

			input.push_back(block[0]);
			input.push_back(block[1]);
			input.push_back(block[2]);
			input.push_back(block[3]);
			input.push_back(block[4]);

			vector<uint32_t>& siphrotext = encrypt_block(initVec, addictedKey, consts);


			transform(block.begin(), block.end(), siphrotext.begin(), siphrotext.begin(), bit_xor<uint32_t>());
			initVec.clear();
			initVec = siphrotext;

			result.push_back(siphrotext[0]);
			result.push_back(siphrotext[1]);
			result.push_back(siphrotext[2]);
			result.push_back(siphrotext[3]);
			result.push_back(siphrotext[4]);

			delete &siphrotext;

			if (!labe) {
				block = create_special_block();

				input.push_back(block[0]);
				input.push_back(block[1]);
				input.push_back(block[2]);
				input.push_back(block[3]);
				input.push_back(block[4]);

				siphrotext = encrypt_block(initVec, addictedKey, consts);
				transform(block.begin(), block.end(), siphrotext.begin(), siphrotext.begin(), bit_xor<uint32_t>());
				result.push_back(siphrotext[0]);
				result.push_back(siphrotext[1]);
				result.push_back(siphrotext[2]);
				result.push_back(siphrotext[3]);
				result.push_back(siphrotext[4]);
				delete &siphrotext;
				break;
			}
			if (fin.eof()) {
				break;
			}
			if (!param) break;
		}

		result.shrink_to_fit();

		fin.close();
		delete consts;
		delete &addictedKey;

		ofstream foutp = Open_File_Write(outFilename);
		writeFH(foutp, fh);
		writeIH(foutp, ih);

		for (size_t i = 0; i < result.size(); i++) {
			char c = result[i] >> MOVE_RIGHT_F;
			foutp.write((char*)&c, 1);
			c = (result[i] << MOVE_RIGHT_T) >> MOVE_RIGHT_F;
			foutp.write((char*)&c, 1);
			c = (result[i] << MOVE_RIGHT_S) >> MOVE_RIGHT_F;
			foutp.write((char*)&c, 1);
			c = (result[i] << MOVE_RIGHT_F) >> MOVE_RIGHT_F;
			foutp.write((char*)&c, 1);
		}
		foutp.close();

		correlation(input, result);
		dispersion(result);
	}
}