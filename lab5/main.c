#include "headers.h"

void* malloc(size_t bytes);
static const int INF = 1e9;

byte get_bit(byte num, size_t index, size_t bit_count) { // from left to right and [0, 7]
	return !((num >> (7 - index)) & 1);
}

void print_gen(byte** begin, size_t amount, int width, int height) {
	byte** cur = begin;
	for (int k = 0; k < amount; ++k) {
		byte** tmp = game_step(cur, width, height);
		cur = tmp;
		for (int i = 0; i < height; ++i) {
			for (int j = 0; j < width; ++j)
				printf("%d", cur[i][j]);
			printf("\n");
		}
		printf("\n\n\n");
	}
}
int func(int arg1, unsigned long arg2) {
	return 0;
}

int typedef int_t, * intp_t, (*fp)(int, unsigned long), arr_t[10];
int main(int argc, char* argv[]) {
	FILE* input;
	FILE* output;
	if (argc == 1)
		return -1;
	else
		input = fopen(argv[2], "rb");
	
	if (!input) {
		printf("ERROR!\n");
		return -1;
	}
	int freq = 10;
	int mx_iter = INF;
	
	char* dir_name = (char*)malloc(sizeof(char) * 50);
	
	if (argc != 1) {
		strcpy(dir_name, argv[4]);
		if (argc < 8 && argc >= 6 && !strcmp(argv[5], "--max_iter")) {
			mx_iter = atoi(argv[6]);
		}
		else if (argc < 8 && argc >= 6 && !strcmp(argv[5], "--dump_freq")) {
			freq = atoi(argv[6]);
		}
		else if (argc >= 8 && !strcmp(argv[5], "--max_iter")) {
			mx_iter = atoi(argv[6]);
			freq = atoi(argv[8]);
		}
		else if (argc >= 8 && !strcmp(argv[5], "--dump_freq")) {
			mx_iter = atoi(argv[8]);
			freq = atoi(argv[6]);
		}
		//printf("I AM HERE!\n");
	}
	char* path = (char*)malloc(sizeof(char) * 200);
	strcpy(path, "mkdir ");
	system(strcat(path, dir_name));
	
	strcat(dir_name, "\\");
	if (argc >= 9) {
		printf("%s %s %s", argv[4], argv[6], argv[8]);
		printf("%d %d\n", mx_iter, freq);
	}

	// ANALYZING COMMAND LINE INSTRUCTIONS HAS JUST FINISHED :)
	char* file_name = malloc(sizeof(char) * 200);
	double prev_time = 0;
	strcpy(file_name, dir_name); // название директории
	//strcat(file_name, "\\");
	free(path);
	double delta = 1.0 / freq * CLOCKS_PER_SEC;
	int counter = 0;
	FILE* prev = 0;
	FILE* cur = 0;
	char* temp_buffer = malloc(sizeof(char) * 4);
	while (counter < mx_iter) {
		char* prev_file = 0;
		char* cur_file = malloc(sizeof(char) * 200);
		strcpy(cur_file, file_name);
		strcat(cur_file, _itoa(counter, temp_buffer, 10));
		strcat(cur_file, ".bmp");
		Sleep(1.0 / freq * CLOCKS_PER_SEC);
		if (counter == 0) {
			output = fopen(cur_file, "wb");
		}
		else {
			char* prev_file = malloc(sizeof(char) * 200);
			strcpy(prev_file, file_name);
			strcat(prev_file, "\\");
			strcat(prev_file, _itoa(counter - 1, temp_buffer, 10));
			strcat(prev_file, ".bmp");
			input = fopen(prev_file, "rb");
			output = fopen(cur_file, "wb");
			free(prev_file);
		}
		free(cur_file);
		BMP_FILE_HEADER* bfh = (BMP_FILE_HEADER*)malloc(sizeof(BMP_FILE_HEADER));
		BMP_INFO_HEADER* bih = (BMP_INFO_HEADER*)malloc(sizeof(BMP_INFO_HEADER));
		readHeader(bfh, input); // конец в d
		readInfo(bih, input);
		size_t bit_in_width = (bih->biWidth * bih->biBitCount);
		size_t real_width = (bit_in_width % 32 == 0
			? bit_in_width
			: bit_in_width + (32 - (bit_in_width % 32)));
		byte** pixels = malloc(sizeof(byte*) * bih->biHeight);
		for (int i = 0; i < bih->biHeight; ++i)
			pixels[i] = malloc(sizeof(byte) * real_width);

		fseek(input, 0, SEEK_SET);

		fseek(input, bfh->bfOffBits, SEEK_SET);

		for (int i = bih->biHeight - 1; i >= 0; --i) {
			byte* buffer = malloc(sizeof(byte) * (real_width / 8));
			int j = 0;
			while (j < ceil((double)bih->biWidth / 8)) {
				fread(buffer + j, sizeof(byte), 1, input);
				++j;
			}

			while (j < real_width / 8) {
				fread(buffer + j, sizeof(byte), 1, input);
				++j;
			}
			int line_index = 0; // индекс ячейки в двумерном массиве в i-й строке
			int worthful_bits = bih->biBitCount * bih->biWidth;
			for (j = 0; j < worthful_bits; ++j) {
				pixels[i][j / bih->biBitCount] = get_bit(buffer[j / 8], j % 8, bih->biBitCount);
			}
			free(buffer);
		}

		/*printf("END OF READING FILE!\n");

		for (int i = 0; i < bih->biHeight; ++i) {
			for (int j = 0; j < bih->biWidth; ++j)
				printf("%d", pixels[i][j]);
			printf("\n");
		}
		printf("\n\n");*/

		pixels = game_step(pixels, bih->biWidth, bih->biHeight);

		write_generation(pixels, bih->biHeight, bih->biWidth, input, output, bfh->bfOffBits);
		for (int i = 0; i < bih->biHeight; ++i)
			free(pixels[i]);
		free(pixels);
		fclose(input);
		++counter;
	}
	free(file_name);
	return 0;
}