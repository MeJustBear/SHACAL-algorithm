#pragma once

#include<iostream>
#include<fstream>

#define bmp 19778


#pragma pack(push,1)
/*@brief
first header in bmp type
*/
typedef struct {
	unsigned short bfType; //2
	unsigned int   bfSize; //4(6)
	unsigned short    bfReserved1;//2(8) 
	unsigned short    bfReserved2; //2(10)
	unsigned int   bfOffBits; //4(14)
}BMPFILEHEADER;//14
#pragma pack(pop)


#pragma pack(push,1)
/*@brief
second header in bmp type
*/
typedef struct {
	unsigned int  bi_Size;	//18
	unsigned int   bi_Width; //22
	unsigned int   bi_Height; //26
	unsigned short   bi_Planes; //28 
	unsigned short   bi_BitCount; //30
	unsigned int  bi_Compression; //34
	unsigned int  bi_SizeImage; //38
	int   bi_X; //42
	int   bi_Y; //46
	unsigned int  bi_ClrUsed; // 50
	unsigned int  bi_ClrImportant; //54
}BMP, BMPINFOHEADER;//40
#pragma pack(pop)

/**
@brief
*/
std::ifstream Open_File_Read(std::string filename);

/**
@brief
*/
std::ofstream Open_File_Write(std::string filename);

/**
@brief
*/
BMPFILEHEADER readFH(std::ifstream& stream);

/**
@brief
*/
void writeFH(std::ofstream & stream, BMPFILEHEADER bf);

/**
@brief
*/
BMPINFOHEADER readIH(std::ifstream& stream);

/**
@brief
*/
void writeIH(std::ofstream & stream, BMPINFOHEADER bi);



/*
#pragma pack(push,1)
/*@brief
Color structure for simple usability
*//*
typedef struct COLOR_pix {
	uint8_t B, G, R;
}COLOR;
#pragma pack(pop)
*/

/*@brief
Class which include one picture in one exemplar of BMP24
*//*
class Image24 {

private:
	BMPFILEHEADER fileheader;
	BMPINFOHEADER infoheader;
	std::vector<std::vector<uint8_t>> map;

public:

	Image24(BMPFILEHEADER fh, BMPINFOHEADER ih) {
		fileheader = fh;
		infoheader = ih;
		map = std::vector<std::vector<uint8_t>>(ih.bi_Height);
	}

	Image24() {
	}

	~Image24() {
		map.clear();
	}

	void set_headers(BMPFILEHEADER fh, BMPINFOHEADER ih) {
		fileheader = fh;
		infoheader = ih;
		map = std::vector<std::vector<uint8_t>>(ih.bi_Height);
	}

	void set_map(std::ifstream& stream);
};*/

/*@brief
Class which include one picture in one exemplar of BMP8
*//*
class Image8 {

private:
	BMPFILEHEADER fileheader;
	BMPINFOHEADER infoheader;
	std::vector<std::vector<uint8_t>> map;

public:

	Image8(BMPFILEHEADER fh, BMPINFOHEADER ih) {
		fileheader = fh;
		infoheader = ih;
		map = std::vector<std::vector<uint8_t>>(ih.bi_Height);
	}

	Image8() {
	}

	~Image8() {
		map.clear();
	}

	void set_headers(BMPFILEHEADER fh, BMPINFOHEADER ih) {
		fileheader = fh;
		infoheader = ih;
		map = std::vector<std::vector<uint8_t>>(ih.bi_Height);
	}

	void set_map(std::ifstream& stream);

};*/