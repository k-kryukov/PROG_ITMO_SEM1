#include "headers.h"

static bool compress = false;

int scan_file(ARC_FILE* file, FILE* input) {
	fread(&file->name_size, sizeof(size_t), 1, input);

	file->name = (char*)malloc(sizeof(char) * (file->name_size + 1));
	if (file->name == 0) 
		return -1;

	for (int i = 0; i < file->name_size; ++i)
		fread(file->name + i, sizeof(char), 1, input);
	file->name[file->name_size] = '\0';

	fread(&file->size, sizeof(size_t), 1, input);
	
	file->bytes = (byte*)malloc(sizeof(byte) * file->size);
	for (int i = 0; i < file->size; ++i)
		fread(file->bytes + i, sizeof(byte), 1, input);
}

void print_list(char* arc_name) {
	FILE* arc = fopen(arc_name, "rb");
	if (!arc) {
		printf("ARCHIVE ISN'T FOUND!");
		return;
	}

	size_t amount;
	fread(&amount, sizeof(size_t), 1, arc);

	for (int i = 0; i < amount; ++i) {
		ARC_FILE cur_file;
		scan_file(&cur_file, arc);
		printf("%d. %s\n", i + 1, cur_file.name);
	}

	fclose(arc);
}

void extract_arc(char* arc_name) {
	FILE* arc = fopen(arc_name, "rb");
	if (!arc) {
		printf("ARCHIVE ISN'T FOUND!");
		return;
	}

	size_t amount;
	fread(&amount, sizeof(size_t), 1, arc);

	for (int i = 0; i < amount; ++i) {
		ARC_FILE cur_file;
		scan_file(&cur_file, arc);

		char* real_name = (char*)malloc(sizeof(char) * cur_file.name_size);
		strcpy(real_name, cur_file.name);

		FILE* this_file = fopen(real_name, "wb");
		for (int i = 0; i < cur_file.size; ++i)
			fwrite(cur_file.bytes + i, sizeof(byte), 1, this_file);
		fclose(this_file);

		int len = strlen(real_name);
		if (!compress || strncmp(real_name + len - 3, "txt", 3) != 0)
			continue;

		Decode(real_name, "decoded.txt");

		this_file = fopen(real_name, "wb");
		FILE* decoded = fopen("decoded.txt", "rb");
		unsigned char buff;

		while (fread(&buff, sizeof(unsigned char), 1, decoded) != 0) {
			fwrite(&buff, sizeof(unsigned char), 1, this_file);
		}

		fclose(decoded);
		fclose(this_file);
	}

	fclose(arc);
}

void write_file(char* file_name, FILE* arc) {
	FILE* cur_file;
	int len = strlen(file_name);
	if (compress && strncmp(file_name + len - 3, "txt", 3) == 0) {
		Encode(file_name, "temp_file.txt");
		cur_file = fopen("temp_file.txt", "rb");
	}
	else
		cur_file = fopen(file_name, "rb");
	
	if (!cur_file) {
		printf("FILE WASN'T OPENED!");
	}

	size_t file_size = 0;
	fseek(cur_file, 0, SEEK_END);
	file_size = ftell(cur_file);
	fseek(cur_file, 0, SEEK_SET);

	size_t name_size = strlen(file_name);
	fwrite(&name_size, sizeof(size_t), 1, arc);

	for (int i = 0; i < name_size; ++i)
		fwrite(file_name + i, sizeof(char), 1, arc);
	fwrite(&file_size, sizeof(size_t), 1, arc);

	for (int i = 0; i < file_size; ++i) {
		byte* buffer = malloc(sizeof(byte));
		if (!fread(buffer, sizeof(byte), 1, cur_file))
			printf("NO BYTES HAS READ!");
		fwrite(buffer, sizeof(byte), 1, arc);

		free(buffer);
	}
	fclose(cur_file);
}

void create_arc(char* arc_name, size_t file_c, char** file_names) {
	FILE* arc = fopen(arc_name, "wb");
	fwrite(&file_c, sizeof(size_t), 1, arc);

	for (int i = 0; i < file_c; ++i)
		write_file(file_names[i], arc);

	fclose(arc);
}