#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <math.h>
#include <windows.h>
#include <string.h>
#include <time.h>
#include <dos.h>

typedef unsigned char byte;
typedef unsigned short word;
typedef uint32_t dword;
typedef uint64_t qword;

typedef struct {
	word bfType;
	dword bfSize;
	word bfReserved1;
	word bfReserved2;
	dword bfOffBits;
} BMP_FILE_HEADER;

typedef struct
{
	dword  biSize;
	dword  biWidth;
	int  biHeight;
	word  biPlanes;
	word  biBitCount;
	dword biCompression;
	dword biSizeImage;
	int64_t biXPelsPerMeter;
	int64_t biYPelsPerMeter;
	word biClrUsed;
	word biClrImportant;
} BMP_INFO_HEADER;

void readHeader(BMP_FILE_HEADER* h, FILE* stream);
void readInfo(BMP_INFO_HEADER* h, FILE* stream);
void fillLine(byte* line, int line_width);
byte** game_step(byte** field, size_t w, size_t h);
void write_generation(byte** pixels, int h, int w, FILE* in, FILE* out, size_t offset);