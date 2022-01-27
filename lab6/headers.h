#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <locale.h>
#define DEBUG
#define ALPH_SIZE 256
#define MAX_CODES 16

typedef unsigned char byte;

typedef struct {
	size_t name_size;
	char* name;
	size_t size;
	byte* bytes;

} ARC_FILE;

typedef struct tagTree Tree;

struct tagTree
{
	int freq;
	int c;
	Tree* left, * right;
};

void print_list(char* arc_name);
void extract_arc(char* arc_name);
void create_arc(char* arc_name, size_t file_c, char** file_names);
int Encode(char* file_name, char* output);
int Decode(char* input_name, char* output_name);