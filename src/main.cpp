#include"Header.h"
#include"HeaderForBmp.h"



using namespace std;

//using namespace ecb;
using namespace cbc;
//using namespace cfb;
//using namespace ofb;

#define BMP
//#define TEXT
#define DEBUG
//#define DEBUG_1
//#define DEBUG_2
#define DEBUG_3

void read_key(vector<uint32_t>& key, string filename);

int main() {
#ifndef DEBUG
	cout << "wanna decypt(d) or encrypt(e)?" << '\n';
	char c;
	cin >> c;
#endif

#ifdef BMP
#ifdef DEBUG_1
	ifstream fin = Open_File_Read("..\\..\\bmps\\4.bmp");
	readFH(fin);
	readIH(fin);
	encrypt(fin, "..\\bmp_key.txt", "..\\output_shacal.bmp", "..\\bmp_vec.txt");

#endif // DEBUG_1

#ifdef DEBUG_2
	ifstream fin("..\\output_shacal.bmp", ios::binary);

	readFH(fin);
	readIH(fin);

	vector<uint32_t> key(KEY_LENGHT);
	read_key(key, "..\\bmp_key.txt");

	decrypt(fin, key, "..\\output.bmp", "..\\bmp_vec.txt");

#endif // DEBUG_2

#ifdef DEBUG_3

	ifstream fin = Open_File_Read("..\\..\\bmps\\4.bmp");
	readFH(fin);
	readIH(fin);
	encrypt(fin, "..\\bmp_key.txt", "..\\output_shacal.bmp", "..\\bmp_vec.txt");
	fin.close();
	fin.clear();
	
	fin.open("..\\output_shacal.bmp", ios::binary);

	BMPFILEHEADER fh = readFH(fin);
	BMPINFOHEADER ih = readIH(fin);

	vector<uint32_t> key(KEY_LENGHT);
	read_key(key, "..\\bmp_key.txt");

	decrypt(fin, key, "..\\output.bmp", "..\\bmp_vec.txt");
	fin.close();
	fin.clear();
#endif // DEBUG_3

	//fin.close();
#endif // BMP

#ifdef TEXT	

#ifdef DEBUG_2

	string strDecr("..\\output.shacal");
	vector<uint32_t> key(KEY_LENGHT);
	read_key(key, "..\\txt_key.txt");
	decrypt(strDecr, key, "..\\output.txt");

#endif // DEBUG

#ifdef DEBUG_1
	string strEncr("..\\..\\texts\\3.txt");
	encrypt(strEncr);
#endif // DEBUG_1

#ifdef DEBUG_3

	string strEncr("..\\..\\texts\\3.txt");
	encrypt(strEncr, "..\\txt_key.txt", "..\\output_shacal.txt", "..\\txt_vec.txt");

	string strDecr("..\\output_shacal.txt");
	vector<uint32_t> key(KEY_LENGHT);
	read_key(key, "..\\txt_key.txt");
	decrypt(strDecr, key, "..\\output.txt", "..\\txt_vec.txt");

#endif // !DEBUG_3
#endif // TEXT

}

void read_key(vector<uint32_t>& key, string filename) {
	std::ifstream input(filename, std::ios::binary);
	std::vector<unsigned char> text(
		(std::istreambuf_iterator<char>(input)),
		(std::istreambuf_iterator<char>()));
	input.close();
	size_t j = 0;

	for (size_t i = 0; i < text.size() && j < KEY_LENGHT; i++, j++) {
		char str[8];
		uint8_t cur = 0;
		while (text[i] != ' ' && i < text.size()) {
			str[cur] = text[i];
			i++;
			cur++;
			if (i == text.size()) break;
		}
		char* pEnd;
		key[j] = static_cast<uint32_t>(strtoul(str, &pEnd, 16));
	}
	input.close();
}
