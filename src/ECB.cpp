#include"ServiceHeader.h"
#include"Header.h"
#include"HeaderForBmp.h"

using namespace std;

namespace ecb {

	void encrypt(const string& inFile, const string& outFile,
		vector<uint32_t>& key, vector<uint32_t>& /*iv*/,
		const uint32_t* consts, bool preserveBmp, bool stats) {
		ifstream fin = Open_File_Read(inFile);
		BMPFILEHEADER fh{};
		BMPINFOHEADER ih{};
		size_t startPos = 0;
		if (preserveBmp) {
			fin.seekg(0, ios::beg);
			fh = readFH(fin);
			ih = readIH(fin);
			startPos = static_cast<size_t>(fin.tellg());
		}
		fin.seekg(0, ios::end);
		size_t maxsize = fin.tellg();
		size_t currentPointer = startPos;
		fin.seekg(startPos, ios::beg);
		bool param = true;
		bool paramd = true;

		vector<uint32_t> addictedKey = key_addiction(key);
		vector<uint32_t> block(BLOCKS_SIZE);
		vector<uint32_t> result;
		result.reserve(1000);
		vector<uint32_t> input;
		input.reserve(1000);

		bool labe = true;
		while (labe) {
			labe = !read_block(fin, block, maxsize, &currentPointer, &param, &paramd);

			input.insert(input.end(), block.begin(), block.end());

			vector<uint32_t> ct = encrypt_block(block, addictedKey, consts);
			result.insert(result.end(), ct.begin(), ct.end());

			if (!labe) {
				vector<uint32_t> sp = create_special_block();
				input.insert(input.end(), sp.begin(), sp.end());
				sp = encrypt_block(sp, addictedKey, consts);
				result.insert(result.end(), sp.begin(), sp.end());
				break;
			}
			if (fin.eof()) break;
			if (!param) break;
		}

		result.shrink_to_fit();
		fin.close();

		write_output(outFile, result, preserveBmp, fh, ih, {});

		if (stats) {
			correlation(input, result);
			dispersion(result);
		}
	}

	void decrypt(const string& inFile, const string& outFile,
		vector<uint32_t>& key, vector<uint32_t>& /*iv*/,
		const uint32_t* consts, bool preserveBmp) {
		ifstream fin = Open_File_Read(inFile);
		BMPFILEHEADER fh{};
		BMPINFOHEADER ih{};
		size_t startPos = 0;
		if (preserveBmp) {
			fin.seekg(0, ios::beg);
			fh = readFH(fin);
			ih = readIH(fin);
			startPos = static_cast<size_t>(fin.tellg());
		}
		fin.seekg(0, ios::end);
		size_t maxsize = fin.tellg();
		size_t currentPointer = startPos;
		fin.seekg(startPos, ios::beg);
		bool param = true;
		bool paramd = true;

		vector<uint32_t> addictedKey = key_addiction(key);
		vector<uint32_t> block(BLOCKS_SIZE);
		vector<uint32_t> result;
		result.reserve(1000);

		while (!fin.eof()) {
			read_block(fin, block, maxsize, &currentPointer, &param, &paramd);
			if (!paramd) break;

			vector<uint32_t> pt = decrypt_block(block, addictedKey, consts);
			result.insert(result.end(), pt.begin(), pt.end());

			if (currentPointer == maxsize) break;
		}

		result.shrink_to_fit();
		fin.close();

		vector<uint8_t> tail = finalise_res(result);
		write_output(outFile, result, preserveBmp, fh, ih, tail);
	}

}
