#include"HeaderForBmp.h"

using namespace std;

ifstream Open_File_Read(string filename){
	ifstream fin(filename, ios::binary);
	if (!fin.is_open()) {
		throw std::exception("Input file not found");
	}
	return fin;
}

ofstream Open_File_Write(string filename){
	ofstream fout(filename, ios::binary);
	if (!fout.is_open()) {
		throw std::exception("Output file not found");
	}
	return fout;
}

BMPFILEHEADER readFH(ifstream & stream){
	BMPFILEHEADER fh;
	stream.read((char*)&fh, sizeof(BMPFILEHEADER));
	if (fh.bfType != bmp) {
		throw std::exception("File is not BMP");
	}
	return fh;
}

void writeFH(ofstream & stream, BMPFILEHEADER bf){
	stream.write((char*)&bf, sizeof(BMPFILEHEADER));
}

BMPINFOHEADER readIH(ifstream & stream){
	BMPINFOHEADER ih;
	stream.read((char*)&ih, sizeof(BMPINFOHEADER));
	return ih;
}

void writeIH(ofstream & stream, BMPINFOHEADER bi){
	stream.write((char*)&bi, sizeof(BMPINFOHEADER));
}
