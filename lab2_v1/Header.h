#pragma once

#include<vector>
#include<iostream>
#include<fstream>
#include<random>
#include<chrono>
#include<math.h>

#define KEY_LENGHT 16
#define FULL_KEY_LENGHT 80
#define INT_SIZE 32

#define MOVE_RIGHT_F 24
#define MOVE_RIGHT_S 16
#define MOVE_RIGHT_T 8
#define BLOCKS_SIZE 5
#define BLOCK_SIZE 4
#define CONSTS_SIZE 5
#define ZERO_BLOCK 19

namespace ecb {

	/**
	@brief
	ecb main decrypt function at ecb mode (only for files withot headers).
	Args:
	std::string decryptFileName- name of file to decrypt
	std::vector<uin32_t> key - 16 values vector, which used to encrypt open text
	All files decrypted with name output.(expansion)
	*/
	void decrypt(std::string decryptFileName, std::vector<uint32_t>& key, std::string outputFilename, std::string vectorFilename);

	/**
	@brief
	main encrypt function at ecb mode (only for files withot headers)
	Args:
	std::string destinationFileName - name of file to encrypt
	All files encrypted with name output.schacal
	*/
	void encrypt(std::string destinationFileName, std::string keyFilename, std::string outputFilename, std::string vectorFilename);

	/**
	brief
	main decrypt function at ecb mode (have to prepare stream by reading all headers)
	std::ifstream& fin - input file stream to read blocks from encrypted file
	std::vector<uin32_t> key - 16 values vector, which used to encrypt open text
	All files decrypted with name output.(expansion)
	*/
	void decrypt(std::ifstream& instream, std::vector<uint32_t>& key, std::string outFilename, std::string vectorFilename);

	/*@brief
	main encrypt function at ecb mode (have to prepare stream by reading all headers))
	*/
	void encrypt(std::ifstream& fin, std::string keyFilename, std::string outFilename, std::string vectorFilename);
}

namespace cbc {

	/**
	@brief

	*/
	void decrypt(std::string decryptFileName, std::vector<uint32_t>& key, std::string outputFilename, std::string vectorFilename);

	/**
	@brief

	*/
	void encrypt(std::string destinationFileName, std::string keyFilename, std::string outputFilename, std::string vectorFilename);

	/**
	brief

	*/
	void decrypt(std::ifstream& instream, std::vector<uint32_t>& key, std::string outFilename, std::string vectorFilename);

	/*@brief

	*/
	void encrypt(std::ifstream& fin, std::string keyFilename, std::string outFilename, std::string vectorFilename);
}

namespace cfb {

	/**
	@brief

	*/
	void decrypt(std::string decryptFileName, std::vector<uint32_t>& key, std::string outputFilename, std::string vectorFilename);

	/**
	@brief

	*/
	void encrypt(std::string destinationFileName, std::string keyFilename, std::string outputFilename, std::string vectorFilename);

	/**
	brief

	*/
	void decrypt(std::ifstream& instream, std::vector<uint32_t>& key, std::string outFilename, std::string vectorFilename);

	/*@brief

	*/
	void encrypt(std::ifstream& fin, std::string keyFilename, std::string outFilename, std::string vectorFilename);
}

namespace ofb {

	/**
	@brief

	*/
	void decrypt(std::string decryptFileName, std::vector<uint32_t>& key, std::string outputFilename, std::string vectorFilename);

	/**
	@brief

	*/
	void encrypt(std::string destinationFileName, std::string keyFilename, std::string outputFilename, std::string vectorFilename);

	/**
	brief

	*/
	void decrypt(std::ifstream& instream, std::vector<uint32_t>& key, std::string outFilename, std::string vectorFilename);

	/*@brief

	*/
	void encrypt(std::ifstream& fin, std::string keyFilename, std::string outFilename, std::string vectorFilename);
}

/**
@brief
have to generate KEY_LENGHT numbers of int32_t
*/
std::vector<uint32_t>& key_generate();

/**
@brief
have to generate BLOCKS_SIZE numbers of int32_t
*/
std::vector<uint32_t>& vector_init();
