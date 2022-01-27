#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <locale.h>
#include <inttypes.h>
#include <stdbool.h>

bool isLet(char c) {
	return ('a' <= c && c <= 'z') 
		|| ('A' <= c && c <= 'Z') 
		|| ('0' <= c && c <= '9');
}

int main(int argc, char** argv) {
	setlocale(LC_ALL, "Russian");
	
	char* file = argv[2];
	FILE* f;
	f = fopen(file, "rt");
	if (!f) {
		return -1;
	}
	
	int capacity = 1;
	int written = 0;
	char* option = argv[1];
	
	if (argc == 1) { // while debugging with Visual Studio (without command line)
		option = "-l";
		file = "C:\\Users\\kkryu\\source\\repos\\опнцю_1\\Debug\\test.txt";
	}
	
	char* s = (char*)malloc(sizeof(char) * (capacity + 1));
	s[1] = '\0';
	while (!feof(f)) {
		char c = fgetc(f);
		if (c == EOF)
			break;
		
		s[written++] = c;
		s[written] = '\0';
		
		if (written == capacity) {
			char* tmp = (char*)malloc(sizeof(char) * (capacity * 2 + 1));
			capacity *= 2;
			for (int i = 0; i < written; i++)
				tmp[i] = s[i];
			tmp[written] = '\0';
			
			free(s);
			s = 0;
			s = tmp;
		}
	}
	fclose(f);
	
	int strCount = 1;
	int wordCount = 0;
	
	if (strcmp(option, "-l") == 0 || strcmp(option, "--lines") == 0) {
		for (int i = 0; i < written; i++) {
			if (i != written - 1 && s[i] == '\n')
				strCount++;
		}
		printf("%i", strCount);
	}
	else if (strcmp(option, "-c") == 0 || strcmp(option, "--bytes") == 0) {
		// size (in bytes)
		f = fopen(file, "rb");
		
		char t;
		int64_t cnt = 0;
		
		while (!feof(f)) {
			t = fgetc(f);
			if (t == EOF)
				break;
			cnt++;
		}
		
		printf("%lli", cnt);
		fclose(f);
	}
	else if (strcmp(option, "-w") == 0 || strcmp(option, "--words") == 0) {
		for (int i = 0; i < written; i++) {
			while (i != written && isLet(s[i]))
				i++;
			
			while (i != written && !isLet(s[i++]));
			
			if (i != written)
				i--;
			wordCount++;
		}
		
		printf("%i", wordCount);
	}
	
	free(s);
	return 0;
}