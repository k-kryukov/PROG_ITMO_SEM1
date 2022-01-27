#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

const int sec_in_month = 2678400;

const int sec_in_day = 86400;

const int sec_in_hour = 3600;

int max(int a, int b) {
	return ((a > b) ? a : b); 
}

bool read_line(FILE* input, char** dest) {
	if (feof(input))
		return false;
	
	int size = 2;
	int already_read = 0;
	char* res = malloc(sizeof(char) * size);
	char c;
	
	while (!feof(input) && (c = fgetc(input)) != '\n') {
		res[already_read++] = c;
		
		if (already_read == size - 1) {
			size *= 2;
			char* temp = malloc(sizeof(char) * size);
			
			for (int i = 0; i < already_read; ++i)
				temp[i] = res[i];
			res[already_read] = '\0';
			free(res);
			res = temp;
		}
	}
	
	res[already_read] = '\0';
	*dest = res;
}

bool error_5xx(char* t) {
	return *t == '5';
}

int get_month(char* s) {
	if (s[0] == 'J' && s[1] == 'a' && s[2] == 'n')
		return 0;
	else if (s[0] == 'F' && s[1] == 'e' && s[2] == 'b')
		return 1;
	else if (s[0] == 'M' && s[1] == 'a' && s[2] == 'r')
		return 2;
	else if (s[0] == 'A' && s[1] == 'p' && s[2] == 'r')
		return 3;
	else if (s[0] == 'M' && s[1] == 'a' && s[2] == 'y')
		return 4;
	else if (s[0] == 'J' && s[1] == 'u' && s[2] == 'n')
		return 5;
	else if (s[0] == 'J' && s[1] == 'u' && s[2] == 'l')
		return 6;
	else if (s[0] == 'A' && s[1] == 'u' && s[2] == 'g')
		return 7;
	else if (s[0] == 'S' && s[1] == 'e' && s[2] == 'p')
		return 8;
	else if (s[0] == 'O' && s[1] == 'c' && s[2] == 't')
		return 9;
	else if (s[0] == 'N' && s[1] == 'o' && s[2] == 'v')
		return 10;
	else
		return 11;
}

int get_year(char* s) {
	int res;
	res = 1000 * (s[0] - '0') + 100 * (s[1] - '0') + 10 * (s[2] - '0') + (s[3] - '0');
	return res - 1970;
}

int sec_from_string(char* t) {
	int days = 10 * (t[0] - '0') + (t[1] - '0') - 1;
	int hours = 10 * (t[12] - '0') + (t[13] - '0');
	int minutes = 10 * (t[15] - '0') + (t[16] - '0');
	int seconds = 10 * (t[18] - '0') + (t[19] - '0');
	
	return (seconds + 60 * minutes + hours * sec_in_hour + days * sec_in_day);
}

const char* year_by_number(int y) {
	char* res = (char*)malloc(sizeof(char) * 4);
	
	res[0] = y / 1000 + '0';
	res[1] = (y / 100) % 10 + '0';
	res[2] = (y / 10) % 10 + '0';
	res[3] = y % 10 + '0';
	
	return res;
}

const char* month_by_number(int m) {
	char* s = malloc(sizeof(char) * 3);

	switch (m) {
		case 1:
			return "Jan";
		case 2:
			return "Feb";
		case 3:
			return "Mar";
		case 4:
			return "Apr";
		case 5:
			return "May";
		case 6:
			return "Jun";
		case 7:
			return "Jul";
		case 8:
			return "Aug";
		case 9:
			return "Sep";
		case 10:
			return "Oct";
		case 11:
			return "Nov";
		case 12:
			return "Dec";
	}
}

const char* get_date(int seconds) {
	// WE COUNT SECONDS SINCE 00:00:00 1 JULY 1995
	char* s = (char*)malloc(sizeof(char) * 21);
	strcpy(s, "  /Jul/1995:  :  :  ");
	
	int remaining = seconds;
	int days = remaining / sec_in_day + 1;
	remaining %= sec_in_day;
	int hours = remaining / sec_in_hour;
	remaining %= sec_in_hour;
	int min = remaining / 60;
	remaining %= 60;
	int sec = remaining;
	
	s[0] = (days / 10) + '0';
	s[1] = (days % 10) + '0';
	s[12] = (hours / 10) + '0';
	s[13] = (hours % 10) + '0';
	s[15] = (min / 10) + '0';
	s[16] = (min % 10) + '0';
	s[18] = (sec / 10) + '0';
	s[19] = (sec % 10) + '0';
	
	return s;
}

int main() {
	FILE* input;
	FILE* output;
	input = fopen("access_log_Jul95", "rt");
	output = fopen("result.txt", "wt");
	if (!input) {
		printf("ERROR!");
		return -1;
	}

	const int req_amount = 1891715;
	int* timings = calloc(sec_in_month, sizeof(int));
	int* request = malloc(req_amount * sizeof(int));
	int counter_bad = 0;
	int counter_request = 0;
	int mx = -1;
	int iter = 0;

	while (!feof(input)) {
		char* string = 0;
		bool result = read_line(input, &string);
		int size = strlen(string);
		if (!result)
			break;
		
		int cur_pos = 0;
		while (string[cur_pos] != '[' && cur_pos < size)
			++cur_pos;
		++cur_pos;
		if (cur_pos == size + 1)
			continue;

		int cur_time = sec_from_string(string + cur_pos);
		request[iter++] = cur_time;
		++timings[cur_time];
		
		if (string[size - 2] == ' ' && (string[size - 1] == '-' || string[size - 1] == '0')) {
			int pos_begin = size - 1 - 4;
			if (error_5xx(string + pos_begin)) {
				fprintf(output, "%s\n", string);
				++counter_bad;
			}
		}
		free(string);

		++counter_request;
		if (counter_request % 100000 == 0) {
			printf("%f%% is done!\n", (double)counter_request * 100 / 1900000);
		}
	}

	fclose(input);
	fclose(output);
	// END OF FILE ANALYZING

	printf("There are %d bad requests (with error 5xx)\n\n", counter_bad);
	while (1) {
		printf("Type time_window to analyze file or -1 for leaving!\n");
		int time_window;
		scanf("%i", &time_window);
		if (time_window == -1) {
			printf("Bye bye!\n");
			break;
		}
		// ARRAY'S CHECKING
		int left = 0, right = time_window - 1;
		int how_many_requests = 0;
		int	max_left = left;
		int max_right = right;
		int max_req;

		for (int i = left; i <= right; ++i)
			how_many_requests += timings[i];
		max_req = how_many_requests;
		
		while (right + 1 < sec_in_month) {
			how_many_requests -= timings[left++];
			how_many_requests += timings[++right];
			
			if (max_req < how_many_requests) {
				max_req = how_many_requests;
				max_left = left;
				max_right = right;
			}
		}

		// TWO POINTERS
		int first = 0, second = 0;
		int max_req_2 = -1;
		while (second < req_amount) {
			while (second < req_amount && request[second] - request[first] < time_window) {
				++second;
			}
			
			--second;
			max_req_2 = max(max_req_2, second - first + 1); // length of segment: b - a + 1
			++first;
			if (first >= req_amount)
				break;
		}

		printf("First way: most of requests in period = %d seconds was sent from %s up to %s\n(%d requests!)\n",
			time_window, get_date(max_left), get_date(max_right), max_req);
		printf("Second way: the biggest amount of requests in period = %d seconds was %d!\n",
			time_window, max_req);
	}
	return 0;
}