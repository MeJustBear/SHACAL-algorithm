#include"ServiceHeader.h"
#include"Header.h"
#include"HeaderForBmp.h"

using namespace std;

namespace ecb {

	void decrypt(string decryptFileName, vector<uint32_t>& key, string outputFilename, string vectorFilename) {
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
			vector<uint32_t>& siphrotext = decrypt_block(block, addictedKey, consts);

			result.push_back(siphrotext[0]);
			result.push_back(siphrotext[1]);
			result.push_back(siphrotext[2]);
			result.push_back(siphrotext[3]);
			result.push_back(siphrotext[4]);

			delete &siphrotext;
			block.clear();

			if (currentPointer == maxsize) break;

		}

		result.shrink_to_fit();

		fin.close();

		vector<uint8_t>& tmp = finalise_res(result);

		write_result(result, outputFilename);
		write_result8(tmp, outputFilename);

		delete &addictedKey;
		delete consts;
		delete &tmp;

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
		delete &key;

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

			vector<uint32_t>& siphrotext = encrypt_block(block, addictedKey, consts);

			result.push_back(siphrotext[0]);
			result.push_back(siphrotext[1]);
			result.push_back(siphrotext[2]);
			result.push_back(siphrotext[3]);
			result.push_back(siphrotext[4]);

			delete &siphrotext;

			if (!labe) {
				siphrotext = create_special_block();
				
				input.push_back(siphrotext[0]);
				input.push_back(siphrotext[1]);
				input.push_back(siphrotext[2]);
				input.push_back(siphrotext[3]);
				input.push_back(siphrotext[4]);

				siphrotext = encrypt_block(siphrotext, addictedKey, consts);

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
		/*"..\\output.shacal"*/
		write_result(result, outputFilename);

		correlation(input,result);
		dispersion(result);


		delete &addictedKey;
		delete consts;
		fin.close();
	}

	void decrypt(ifstream& instream, vector<uint32_t>& key, string outFilename, string vectorFilename) {
		size_t pos = instream.tellg();
		instream.seekg(0, ios::beg);
		BMPFILEHEADER fh = readFH(instream);
		BMPINFOHEADER ih = readIH(instream);
		instream.seekg(0, ios::end);
		size_t maxsize = instream.tellg();
		size_t currentPointer = pos;
		instream.seekg(pos, ios::beg);
		bool param = true;
		bool paramd = true;

		vector<uint32_t>& addictedKey = key_addiction(key);

		uint32_t* const consts = read_consts();

		vector<uint32_t> block(BLOCKS_SIZE);

		vector<uint32_t> result;
		result.reserve(1000);

		bool label = true;

		while (!instream.eof()) {
			label = !read_block(instream, block, maxsize, &currentPointer, &param, &paramd);

			if (!paramd) break;
			vector<uint32_t>& siphrotext = decrypt_block(block, addictedKey, consts);

			result.push_back(siphrotext[0]);
			result.push_back(siphrotext[1]);
			result.push_back(siphrotext[2]);
			result.push_back(siphrotext[3]);
			result.push_back(siphrotext[4]);

			delete &siphrotext;

			if (currentPointer == maxsize) break;
		}

		result.shrink_to_fit();


		instream.close();
		instream.clear();

		delete &addictedKey;
		delete consts;

		vector<uint8_t>& tmp = finalise_res(result);

		ofstream foutp = Open_File_Write("..\\output.bmp");
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

		if (!tmp.empty())
			for (uint8_t i = 0; i < tmp.size(); i++) {
				foutp.write((char*)&result[i], 1);
			}
		foutp.close();
		foutp.clear();

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
		cout << endl;
		key.clear();
		delete &key;

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

			vector<uint32_t>& siphrotext = encrypt_block(block, addictedKey, consts);

			result.push_back(siphrotext[0]);
			result.push_back(siphrotext[1]);
			result.push_back(siphrotext[2]);
			result.push_back(siphrotext[3]);
			result.push_back(siphrotext[4]);

			delete &siphrotext;

			if (!labe) {
				vector<uint32_t>& sipher = create_special_block();

				input.push_back(sipher[0]);
				input.push_back(sipher[1]);
				input.push_back(sipher[2]);
				input.push_back(sipher[3]);
				input.push_back(sipher[4]);

				sipher = encrypt_block(sipher, addictedKey, consts);
				result.push_back(sipher[0]);
				result.push_back(sipher[1]);
				result.push_back(sipher[2]);
				result.push_back(sipher[3]);
				result.push_back(sipher[4]);
				delete &sipher;
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
		foutp.clear();

		correlation(input, result);
		dispersion(result);
	}

}