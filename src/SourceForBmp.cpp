#include"HeaderForBmp.h"
#include <stdexcept>

using namespace std;

ifstream Open_File_Read(string filename){
	ifstream fin(filename, ios::binary);
	if (!fin.is_open()) {
		throw std::runtime_error("Input file not found");
	}
	return fin;
}

ofstream Open_File_Write(string filename){
	ofstream fout(filename, ios::binary);
	if (!fout.is_open()) {
		throw std::runtime_error("Output file not found");
	}
	return fout;
}

BMPFILEHEADER readFH(ifstream & stream){
	BMPFILEHEADER fh;
	stream.read((char*)&fh, sizeof(BMPFILEHEADER));
	if (fh.bfType != bmp) {
		throw std::runtime_error("File is not BMP");
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

void write_output(const string& filename, vector<uint32_t>& result, bool preserveBmp,
	const BMPFILEHEADER& fh, const BMPINFOHEADER& ih, const vector<uint8_t>& tail) {
	ofstream fout = Open_File_Write(filename);
	if (preserveBmp) {
		writeFH(fout, fh);
		writeIH(fout, ih);
	}
	for (size_t i = 0; i < result.size(); i++) {
		char c = static_cast<char>(result[i] >> 24);
		fout.write(&c, 1);
		c = static_cast<char>((result[i] << 8) >> 24);
		fout.write(&c, 1);
		c = static_cast<char>((result[i] << 16) >> 24);
		fout.write(&c, 1);
		c = static_cast<char>((result[i] << 24) >> 24);
		fout.write(&c, 1);
	}
	for (size_t i = 0; i < tail.size(); i++) {
		char c = static_cast<char>(tail[i]);
		fout.write(&c, 1);
	}
	fout.close();
}
