/*
typedef struct tagBITMAPFILEHEADER {
	WORD  bfType;      // File type; must be 'BM' (0x4D42) for bitmap files
	DWORD bfSize;      // Size of the file in bytes
	WORD  bfReserved1; // Reserved; must be zero
	WORD  bfReserved2; // Reserved; must be zero
	DWORD bfOffBits;   // Offset, in bytes, from the beginning of the file to the bitmap data (pixel array)
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER {
	DWORD biSize;          // Size of this header in bytes (should be 40 for BITMAPINFOHEADER)
	LONG  biWidth;         // Width of the image in pixels
	LONG  biHeight;        // Height of the image in pixels
	WORD  biPlanes;        // Number of color planes (must be 1)
	WORD  biBitCount;      // Number of bits per pixel (color depth)
	DWORD biCompression;   // Compression method (0 = BI_RGB, no compression)
	DWORD biSizeImage;     // Size of the image data in bytes (may be 0 for uncompressed images)
	LONG  biXPelsPerMeter; // Horizontal resolution (pixels per meter)
	LONG  biYPelsPerMeter; // Vertical resolution (pixels per meter)
	DWORD biClrUsed;       // Number of colors in the color palette (0 = default)F
	DWORD biClrImportant;  // Number of important colors (0 = all colors are important)
} BITMAPINFOHEADER;

*/

#include <iostream>
#include <fstream>
#include <string>
#include "windows.h"
using namespace std;

const int NUM_INPUT_COLORS = 64;
struct RGB_NAME {
	int red;
	int green;
	int blue;
	string colorName;
	int count = 0;
};

int main(int argc, char* argv[])
{
	cout << "CS230 Lab 6 - Reading a Bitmap File\n";
	BITMAPFILEHEADER bmfh;
	BITMAPINFOHEADER bmih;
	char PictureFileIN[] = "C:\\Users\\hasti\\OneDrive\\Documents\\education\\CS230\\Lab 6\\WikipediaMonaLisa.bmp";

	// Define and read in the input file.
	ifstream bmpIn(PictureFileIN, ios::in + ios::binary);
	if (!bmpIn) {
		cout << "...could not open file" << PictureFileIN << ", ending.";
		return -1;
	}
	char sysDirCmd[256] = "DIR ";
	strcat_s(sysDirCmd, 250, PictureFileIN);
	system(sysDirCmd);
	bmpIn.read((char*)&bmfh, sizeof(bmfh));
	if (bmpIn.fail() || bmpIn.bad()) {
		cout << "Error reading input file, ending\n";
		return(-1);
	}
	cout << "BMFH Size:" << bmfh.bfSize << " Offset:" << bmfh.bfOffBits << '\n';
	bmpIn.read((char*)&bmih, sizeof(bmih));
	if (bmpIn.fail() || bmpIn.bad()) {
		cout << "Error reading BITMAPINFOHEADER, ending\n";
		return -1;
	}

	cout << "Width:" << bmih.biWidth << '\n';


	bmih.biSize = 40;
	cout << "Width:" << bmih.biWidth << '\n';
	cout << "Height:" << bmih.biHeight << '\n';
	cout << "Planes:" << bmih.biPlanes << '\n';
	cout << "Bit Count:" << bmih.biBitCount << '\n';
	cout << "Compression:" << bmih.biCompression << '\n';
	cout << "Size Image:" << bmih.biSizeImage << '\n';
	cout << "Pels per x meter:" << bmih.biXPelsPerMeter << '\n';
	cout << "Pels per y meter:" << bmih.biYPelsPerMeter << '\n';
	cout << "Colors Used:" << bmih.biClrUsed << '\n';
	cout << "Important Colors:" << bmih.biClrImportant << '\n';
	if (bmih.biClrUsed != 0) {
		cout << "Colors used not zero - this program does not handle color tables at present - ending\n";
		return(0);
	}

	//========================================================================

	DWORD imageSize = bmih.biSizeImage;
	LPVOID imageData;
	ifstream ColorFileIn;
	RGB_NAME searchColorArr[NUM_INPUT_COLORS];
	uint8_t* pBitMap;
	int numColorsFound = 0;


	ColorFileIn.open("C:\\Users\\hasti\\iCloudDrive\\My Documents\\Educational_\\Pierce college\\CS 230\\homework\\CC230_Lab6_readbitmap\\Temp\\Colors64.txt");
	if (ColorFileIn.fail() || ColorFileIn.bad()) {
		cout << "Error reading input file, ending\n";
		return(-1);
	}
	imageData = VirtualAlloc(
		NULL,
		imageSize,
		MEM_COMMIT | MEM_RESERVE,
		PAGE_READWRITE
	);
	if (imageData == NULL) {
		cout << "VirtualAlloc failed, ending\n";
		return -1;
	};
	//read bitmap into imageData
	bmpIn.seekg(bmfh.bfOffBits, ios::beg);
	bmpIn.read((char*)imageData, imageSize);
	if (bmpIn.fail() || bmpIn.bad()) {
		cout << "Error reading image data, ending\n";
		VirtualFree(imageData, 0, MEM_RELEASE);
		return -1;
	}
	for (int i = 0; i < NUM_INPUT_COLORS; i++) {
		ColorFileIn >> searchColorArr[i].red >> searchColorArr[i].green >> searchColorArr[i].blue;
		getline(ColorFileIn, searchColorArr[i].colorName);
		for (int j = 0; j < searchColorArr[i].colorName.length(); j++){
			if (searchColorArr[i].colorName[j] != ' ') {
				cout << "____________>  " << j << endl;
				searchColorArr[i].colorName = searchColorArr[i].colorName.substr(j, searchColorArr[i].colorName.length() - j);
				j = searchColorArr[i].colorName.length();
			}
		}
		cout << searchColorArr[i].red << " " << searchColorArr[i].green << " " << searchColorArr[i].blue << " " << searchColorArr[i].colorName << endl;
	}
	

	pBitMap = static_cast<uint8_t*>(imageData);
	for (int i = 0; i < NUM_INPUT_COLORS; i++) {
		cout << "checking for color " << searchColorArr[i].colorName << endl;
		for (int j = 0; j < bmih.biHeight * bmih.biWidth; j++) {
			if (pBitMap[3 * j] == searchColorArr[i].blue &&
				pBitMap[3 * j + 1] == searchColorArr[i].green &&
				pBitMap[3 * j + 2] == searchColorArr[i].red) {
				searchColorArr[i].count++;
				cout << "found the color " << searchColorArr[i].colorName << endl;
			}
		}
		if (searchColorArr[i].count > 0) {
			numColorsFound++;
		}
	}
	cout << "\nThe number of colors found in image: " << numColorsFound << endl;

	for (int i = 0; i < NUM_INPUT_COLORS; i++) {
		if (searchColorArr[i].count) {
			cout << searchColorArr[i].colorName << " was found " << searchColorArr[i].count << " times\n";
		}
	}
	VirtualFree(imageData, 0, MEM_RELEASE);
	

	return 0;
}