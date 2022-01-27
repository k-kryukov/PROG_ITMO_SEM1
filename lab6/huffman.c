#include "headers.h"

void Swap(Tree** a, Tree** b) {
	Tree* tmp;
	tmp = *a;
	*a = *b;
	*b = tmp;
	return;
}

Tree* CreateNode(char c, int freq, Tree* left, Tree* right) {
	Tree* t = (Tree*)malloc(sizeof(Tree));
	t->c = c;
	t->freq = freq;
	t->left = left;
	t->right = right;
	return t;
}

void SortForest(Tree* Forest[ALPH_SIZE], int endi) {
	int i = 0, j = 0;
	
	for (i = 0; i < endi; i++) {
		for (j = 0; j < endi - 1; j++) {
			if (Forest[j]->freq < Forest[j + 1]->freq)
				Swap(&Forest[j], &Forest[j + 1]);
		}
	}
}

Tree* BuildTree(int freq[ALPH_SIZE]) {
	int tmp1, tmp2;
	Tree* tmp;
	Tree* forest[ALPH_SIZE] = { NULL };
	int i;
	int c, count = 0;
	
	for (c = 0; c < ALPH_SIZE; c++) {
		if (freq[c] > 0)
			forest[c] = CreateNode(c, freq[c], NULL, NULL);
		else
			forest[c] = CreateNode(c, 0, NULL, NULL);
	}
	
	for (c = 0; c < ALPH_SIZE; c++) {
		if (forest[c]->freq > 0)
			forest[count++] = forest[c];
	}
	
	while (count > 1) {
		SortForest(forest, count);
		tmp1 = forest[count - 1]->freq;
		tmp = forest[count - 2];
		forest[count - 2] = CreateNode(-1, forest[count - 2]->freq + tmp1, forest[count - 1], tmp);
		count--;
	}
	
	return forest[0];
}

void count_freq(FILE* input, int freq[ALPH_SIZE]) {
	fseek(input, 0, SEEK_SET);
	char c;
	
	for (int i = 0; i < ALPH_SIZE; ++i)
		freq[i] = 0;
	
	while ((c = fgetc(input)) != EOF)
		++freq[c];
}

char* getSpaces(size_t size) {
	char* s = malloc(sizeof(char) * (size + 1));
	
	for (int i = 0; i < size; ++i)
		s[i] = ' ';
	s[size] = '\0';
	
	return s;
}

void print_tree(Tree* root, int depth) {
	if (root == NULL)
		return;

	print_tree(root->right, depth + 1);
	if (root->c == -1)
		printf("%s%s\n", getSpaces(depth * 2), "-1");
	else
		printf("%s%c\n", getSpaces(depth * 2), root->c);
	print_tree(root->left, depth + 1);
}

char buffer[MAX_CODES];

void BuildCodes(char** codes, Tree* t, int cnt) {
	if (t->right == NULL && t->left == NULL) {
		buffer[cnt] = 0;
		codes[t->c] = _strdup(buffer);
		return;
	}
	
	buffer[cnt] = '0';
	BuildCodes(codes, t->left, cnt + 1);
	buffer[cnt] = '1';
	BuildCodes(codes, t->right, cnt + 1);
}

unsigned char accum;

int cnt = 0;

void WriteInit(void) {
	cnt = 0;
	accum = 0;
}

void WriteBit(FILE* f, int b) {
	accum |= (b << cnt);
	cnt++;
	
	if (cnt == 8) {
		fwrite(&accum, 1, 1, f);
		WriteInit();
	}
}

void WriteClose(FILE* f) {
	if (cnt > 0)
		fwrite(&accum, 1, 1, f);
}

int Encode(char* file_name, char* output) {
	FILE* input = fopen(file_name, "rt");
	FILE* encoded_file = fopen(output, "wb");
	if (!input || !encoded_file)
		return -1;

	int freq[ALPH_SIZE];
	count_freq(input, freq);
	for (int i = 0; i < ALPH_SIZE; ++i)
		fwrite(freq + i, sizeof(int), 1, encoded_file);

	Tree* t = BuildTree(freq);
	print_tree(t, 0);

	char* codes[ALPH_SIZE];
	for (int i = 0; i < ALPH_SIZE; ++i)
		codes[i] = 0;
	
	if (t->right == NULL && t->left == NULL)
		codes[t->c] = "0";
	else
		BuildCodes(&codes, t, 0);

	fseek(input, 0, SEEK_SET);
	char buf;
	WriteInit();
	
	while ((buf = fgetc(input)) != EOF) {
		for (char* b = codes[buf]; *b != 0; b++)
			WriteBit(encoded_file, *b - 48);
	}
	
	WriteClose(encoded_file);
	fclose(input);
	fclose(encoded_file);
}

int ReadBit(FILE* f) {
	if (cnt == 0) {
		accum = 0;
		fread(&accum, 1, 1, f);
		cnt = 8;
	}
	
	cnt--;
	return ((accum >> (7 - cnt)) & 1);
}

void ReadInit() {
	cnt = 0;
	accum = 0;
}

int Decode(char* input_name, char* output_name) {
	int b;
	unsigned char test = 255;
	unsigned char test2;
	int flag = -1;
	unsigned char testvar;
	int res[ALPH_SIZE];
	Tree* root, * t;
	int i;
	char c;
	
	FILE* input, * output;
	input = fopen(input_name, "rb");
	output = fopen(output_name, "wt");
	
	fread(res, sizeof(int), ALPH_SIZE, input);
	
	root = BuildTree(res);
	t = root;
	i = 0;
	if (root->left == NULL && root->right == NULL) {
		for (i = 0; i < root->freq; i++)
			fprintf(output, "%c", root->c);
		
		return 1;
	}
	
	ReadInit();
	while (i < root->freq) {
		b = ReadBit(input);
		
		if (b == 0) {
			t = t->left;
		}
		else if (b == 1) {
			t = t->right;
		}
		
		if (t->left == NULL && t->right == NULL) {
			fprintf(output, "%c", t->c);
			i++;
			t = root;
		}
	}
	return 1;
}