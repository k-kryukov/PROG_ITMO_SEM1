#include "headers.h"

void readHeader(BMP_FILE_HEADER* h, FILE* stream) {
	fread(&(h->bfType), sizeof(unsigned short), 1, stream);
	fread(&(h->bfSize), sizeof(unsigned int), 1, stream);
	fread(&(h->bfReserved1), sizeof(unsigned short), 1, stream);
	fread(&(h->bfReserved2), sizeof(unsigned short), 1, stream);
	fread(&(h->bfOffBits), sizeof(unsigned int), 1, stream);
}

void readInfo(BMP_INFO_HEADER* h, FILE* stream) {
	fread(&(h->biSize), sizeof(dword), 1, stream);
	fread(&(h->biWidth), sizeof(dword), 1, stream);
	fread(&(h->biHeight), sizeof(int), 1, stream);
	fread(&(h->biPlanes), sizeof(word), 1, stream);
	fread(&(h->biBitCount), sizeof(word), 1, stream);
	fread(&(h->biCompression), sizeof(dword), 1, stream);
	fread(&(h->biSizeImage), sizeof(dword), 1, stream);
	fread(&(h->biXPelsPerMeter), sizeof(int64_t), 1, stream);
	fread(&(h->biYPelsPerMeter), sizeof(int64_t), 1, stream);
	fread(&(h->biClrUsed), sizeof(word), 1, stream);
	fread(&(h->biClrImportant), sizeof(word), 1, stream);
}

bool ok(int i, int j, int mxi, int mxj) {
	return 0 <= i && i < mxi && 0 <= j && j < mxj;
}

int how_many_neigh(byte** field, int y, int x, size_t h, size_t w) {
	int counter = 0;
	
	for (int i = y - 1; i <= y + 1; ++i) {
		for (int j = x - 1; j <= x + 1; ++j) {
			if (i == y && j == x)
				continue;
			
			if (ok(i, j, h, w) && field[i][j])
				++counter;
		}
	}
	
	return counter;
}

byte** game_step(byte** field, size_t w, size_t h) {
	byte** temp_field = (byte**)malloc(sizeof(byte*) * h);

	for (int i = 0; i < h; ++i)
		temp_field[i] = (byte*)malloc(sizeof(byte) * w);

	for (int i = 0; i < h; ++i) {
		for (int j = 0; j < w; ++j) {
			int counter = how_many_neigh(field, i, j, h, w);
			
			if (field[i][j] == 1) { // ALIVE
				if (counter == 2 || counter == 3)
					temp_field[i][j] = 1;
				else
					temp_field[i][j] = 0;
			}
			else if (counter == 3) {
				temp_field[i][j] = 1;
			}
			else
				temp_field[i][j] = 0;
		}
	}
	
	for (int i = 0; i < h; ++i)
		free(field[i]);
	free(field);
	
	return temp_field;
}

void write_generation(byte** pixels, int h, int w, FILE* in, FILE* out, size_t offset) {
	fseek(in, 0, SEEK_SET);
	
	for (int i = 0; i < offset; ++i) {
		byte buf;
		fread(&buf, sizeof(byte), 1, in);
		fwrite(&buf, sizeof(byte), 1, out);
	}

	// НЕОБХОДИМО ЗАПИСАТЬ НАШ ИТОГОВЫЙ МАССИВ В ФАЙЛ ПОБИТОВО
	for (int i = h - 1; i >= 0; --i) { // количество бит, которые нужно записать
		int counter = 0;
		byte accum = 0; // ЧЕРНЫМ КРАСЯТСЯ НУЛЕВЫЕ БИТЫ (ЗАКРАШИВАЮТСЯ)
		int j = 0;
		int already_sent = 0;
		
		while (j < w) {
			accum += (!pixels[i][j] << (7 - (j % 8)));
			
			if ((j + 1) % 8 == 0) {
				fwrite(&accum, sizeof(byte), 1, out);
				already_sent = (already_sent + 1) % 4;
				accum = 0;
			}
			
			++j;
		}
		if (w % 32 != 0) { // досылаем биты до кратности 32 (4 байта)
			if (already_sent == 0) {
				fwrite(&accum, sizeof(byte), 1, out);
				accum = 0;
				fwrite(&accum, sizeof(byte), 1, out);
				fwrite(&accum, sizeof(byte), 1, out);
				fwrite(&accum, sizeof(byte), 1, out);
			}
			else if (already_sent == 1) {
				fwrite(&accum, sizeof(byte), 1, out);
				accum = 0;
				fwrite(&accum, sizeof(byte), 1, out);
				fwrite(&accum, sizeof(byte), 1, out);
			}
			else if (already_sent == 2) {
				fwrite(&accum, sizeof(byte), 1, out);
				accum = 0;
				fwrite(&accum, sizeof(byte), 1, out);
			}
			else if (already_sent == 3) {
				fwrite(&accum, sizeof(byte), 1, out);
			}
		}
	}
	fclose(out);
}