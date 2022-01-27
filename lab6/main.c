#include "headers.h"

enum {
	create, // only create
	extract, // only extract
	list, // only show list
	cmd // program is executed by command line
};

int main(int argc, char* argv[]) {
	setlocale(LC_ALL, "Rus");
	char* arc_name = "arc.arc";
	int what_to_do = cmd; // flag of program behaviour (check beginning of main.c)

	if (argc == 1) {
		printf("Hello, IDE!\n");
	}
	else if (argc == 2) {
		printf("WRONG NUMBER OF COMMAND LINE INSTRUCTION!\n"
			   "READ README.TXT PROPERLY!\n");
	}
	else
		arc_name = argv[2];

	if (what_to_do == list || (argc != 1 && !strcmp(argv[3], "--list"))) {
		print_list(arc_name);
	}
	else if (what_to_do == extract || ((argc != 1) && !strcmp(argv[3], "--extract"))) {
		extract_arc(arc_name);
	}
	else if (what_to_do == create || ((argc != 1) && !strcmp(argv[3], "--create"))) {
		char** file_arr;

		if (argc != 1) {
			file_arr = (char**)malloc(sizeof(char*) * (argc - 4));

			for (int i = 4; i < argc; ++i) {
				file_arr[i - 4] = argv[i];
			}

			create_arc(arc_name, argc - 4, file_arr);
		}
		else if (argc == 1) { // TESTING MODULE
			int n = 3;
			file_arr = (char**)malloc(sizeof(char*) * n);
			file_arr[0] = "test.txt";
			file_arr[1] = "Electric cars.docx";
			file_arr[2] = "Writing Fourth Module.docx";

			create_arc(arc_name, n, file_arr);
		}

	}
	return 0;
}