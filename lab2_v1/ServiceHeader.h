#pragma once

#include<vector>
#include<iostream>
#include<fstream>

template<typename T>
struct square
{
	T operator()(const T& Left, const T& Right) const
	{
		return (Left + Right * Right);
	}
};


/**
@brief
*/
std::vector<uint32_t>& create_special_block();

/**
@brief
*/
std::vector<uint32_t>& key_addiction(std::vector<uint32_t>& key);

/**
@brief
*/
std::vector<uint32_t>& encrypt_block(std::vector<uint32_t>& block, std::vector<uint32_t>& key, const uint32_t* consts);

/**
@brief
*/
std::vector<uint32_t>& decrypt_block(std::vector<uint32_t>& block, std::vector<uint32_t>& key, const uint32_t* consts);

/**
@brief
*/
std::vector<uint8_t>& finalise_res(std::vector<uint32_t>& result);

/**
@brief
*/
uint32_t* read_consts();

/**
@brief
*/
uint32_t cycle_move_left(uint32_t param, int8_t len);

/**
@brief
*/
uint32_t function(uint32_t x, uint32_t y, uint32_t z, uint8_t i);

/**
@brief
*/
uint32_t function(uint32_t x, uint32_t y, uint32_t z, uint8_t i);

/**
@brief
*/
bool read_block(std::ifstream& fin, std::vector<uint32_t>& dest, size_t maxsize, size_t* currentPointer, bool* param, bool* paramd);

/**
@brief
*/
void write_result(std::vector<uint32_t>& result, std::string filename);

/**
@brief
*/
void write_result8(std::vector<uint8_t>& res, std::string filename);

/**
@brief
*/
void read_init_vec(std::vector<uint32_t>& init, std::string filename);

/**
@brief
*/
void dispersion(std::vector<uint32_t>& res);

/**
@brief
*/
void correlation(std::vector<uint32_t>& in, std::vector<uint32_t>& out);