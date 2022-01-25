#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>


#define FRAME_SIZE 10

#define INF 1e9;

char* get_file_name(char* string) {
    int index = 0;
    while (string[index] != '=') {
        ++index;
    }
    ++index;
    return string + index;
}

typedef struct {
    char file_type[3];
    char version;
    char sub_version;
    char flags;
    char size[4];
} Header;

typedef struct {
    char name[4];
    char flags[2];
    char size[4];
    char* info;
    size_t info_size;
} Frame;

enum type_of_output{
    character,
    hex,
    integer
};

bool scanf_frame(Frame* f, FILE* file) {
    if (feof(file))
        return false;
    for (int i = 0; i < 4; ++i) {
        fscanf(file, "%c", f->name + i);
        if (f->name[i] == -1)
            return false;
    }
    for (int i = 0; i < 4; ++i) {
        fscanf(file, "%c", f->size + i);
        if (f->size[i] == -1)
            return false;
    }
    fscanf(file, "%c%c", f->flags, f->flags + 1);
    if (f->flags[0] == -1 || f->flags[1] == -1)
        return false;
    return true;
}

void print_str(char* f, int size, int type) {
    char* ident;
    if (type == character)
        ident = "%c";
    else if (type == hex)
        ident = "%x";
    else if (type == integer)
        ident = "%d";
    else
        ident = "%c";
    int i = 0;
    while (i < size && f[i] == 0)
        ++i;
    for (int i = 0; i < size; ++i) {
        printf(ident, f[i]);
    }
    printf("\n");
}

void print_frame(Frame f) {
    printf("Name: ");
    print_str(f.name, 4, character);
    printf("Flags: ");
    print_str(f.flags, 2, hex);
    printf("Size of info: ");
    print_str(f.size, 4, integer);
    printf("Info: ");
    for (int i = 1; i < f.info_size; ++i)
        fputc(f.info[i], stdout);
    printf("\n");
}

int ctoi(char c) {
    if ('0' <= c && c <= '9')
        return c - '0';
    else if ('a' <= c && c <= 'z')
        return c - 'a' + 10;
    else if ('A' <= c && c <= 'Z')
        return c - 'A' + 10;
    else
        return 0;
}

int size_of_tag(char* size, int amount) {
    int res = 0;
    for (int i = 0; i < amount; ++i) {
        for (int j = 6; j >= 0; --j) {
            res = res << 1;
            res += ((size[i] >> j) & 1);
        }
    }
    return res;
}

int size_of_frame(char* size) {
    int res = 0;
    for (int i = 0; i < 4; ++i) {
        for (int j = 7; j >= 0; --j) {
            res = res << 1;
            res += ((size[i] >> j) & 1);
        }
    }
    return res;
}

#define STOPPER
#define __DEBUG 1

#ifdef __DEBUG
#define STOPPER "return 0"
#endif

char* get_frame_name(char* name) {
    int index = 0;
    while (name[index] != '=')
        ++index;
    ++index;
    return name + index;
}

typedef union {
    size_t s;
    unsigned char b[4];
} U;

int main(int argc, char* argv[]) {
    char* file_name;
    bool debug = false;
    if (argc == 1) { // without cmd
        file_name = "01.Dream On.mp3";
    }
    else {
        file_name = get_file_name(argv[1]);
    }
    FILE* input = fopen(file_name, "rb");
    if (!input) {
        printf("ERROR!");
        return -1;
    }

    int size = INF;
    int global_pos = 0;
    int cur_pos = 0;
    Header h;
    fscanf(input, "%c%c%c%c%c%c%c%c%c%c", h.file_type, h.file_type + 1, h.file_type + 2,
        &h.version, &h.sub_version, &h.flags, h.size, h.size + 1, h.size + 2, h.size + 3);
    int tag_size = size_of_tag(h.size, 4);
    int worthful = 0;
    Frame* arr = (Frame*)malloc(sizeof(Frame));
    int capacity = 1;
    int amount = 0;
    while (global_pos < tag_size) {
        Frame cur;
        if (!scanf_frame(&cur, input))
            break;
        int size = size_of_frame(cur.size);
        if (size != 0)
            ++worthful;
        char* info = malloc(sizeof(char) * size);
        for (int i = 0; i < size; ++i)
            info[i] = fgetc(input);
        cur.info = info;

        cur.info_size = size;
        arr[amount] = cur;
        amount++;
        if (amount == capacity) {
            capacity *= 2;
            Frame* temp_arr = (Frame*)malloc(sizeof(Frame) * capacity);
            for (int i = 0; i < amount; ++i)
                temp_arr[i] = arr[i];
            free(arr);
            arr = temp_arr;
        }
        global_pos += FRAME_SIZE + size; 
    }

    if (!(argc == 1) && !strcmp(argv[2], "--show")) {
        for (int i = 0; i < worthful; ++i) {
            print_frame(arr[i]);
            printf("\n------------\n");
        }
    }
    else if (!(argc == 1) && !strncmp(argv[2], "--get", 5)) {
        char* name = get_frame_name(argv[2]);
        for (int i = 0; i < amount; ++i) {
            if (!strcmp(arr[i].name, name)) {
                print_frame(arr[i]);
                break;
            }
        }
    }
    else if (argc == 1 || !strncmp(argv[2], "--set", 5)) { // !!! (argc = 1)
        //printf("%s, %s;\n I AM HERE\n", argv[2], argv[3]);
        if (argc == 1) {
            argv[2] = "--set=TIT2";
            argv[3] = "--value=Hello Darkness My Old Friend";
        }
        int i = 0;
        while (argv[2][i] != '=')
            ++i;
        ++i;
        int begin_name = i, end_name;
        while (argv[2][i] != '\0')
            ++i;
        end_name = i;
        // [begin, end) - name of frame
        i = 0;
        while (argv[3][i] != '=')
            ++i;
        ++i;
        int begin_value = i;
        while (argv[3][i] != '\0')
            ++i;
        int end_value = i;
        // [begin, end) - value of this;
        /*printf("From %d to %d\n", begin_name, end_name);
        printf("From %d to %d\n", begin_value, end_value);
        for (int i = begin_name; i < end_name; ++i)
            printf("%c", argv[2][i]);
        printf("\n");
        for (int i = begin_value; i < end_value; ++i)
            printf("%c", argv[3][i]);
        printf("\n");*/

        fseek(input, 0, SEEK_END);
        size_t file_size = ftell(input);
        //printf("%d\n", file_size);
        unsigned char* file_data = (unsigned char*)malloc(sizeof(char) * file_size);

        fseek(input, 0, SEEK_SET);
        unsigned char cur_byte;
        size_t index = 0;
        for (size_t index = 0; index < file_size; ++index) {
            fread(&cur_byte, sizeof(unsigned char), 1, input);
            file_data[index] = cur_byte;
        }

        fclose(input);
        FILE* output = fopen(file_name, "wb");

        for (size_t i = 0; i < file_size - (end_name - begin_name);) {
            if (!strncmp(file_data + i, argv[2] + begin_name, end_name - begin_name)) {
                size_t prev_size = size_of_frame(file_data + i + 4);
                U u;
                fwrite(argv[2] + begin_name, sizeof(unsigned char),
                       end_name - begin_name, output);
                i += end_name - begin_name;
                size_t tag_size = end_value - begin_value + 1;
                u.s = tag_size;
                // + 1 because of zero at the beginning of the info
                //unsigned char prev_size[4];
                for (int j = 3; j >= 0; --j) {
                    //printf("%d ", u.b[j]);
                    fwrite(&u.b[j], sizeof(unsigned char), 1, output);
                }
                i += 4;
                cur_byte = 0;
                fwrite(&cur_byte, sizeof(unsigned char), 1, output);
                fwrite(&cur_byte, sizeof(unsigned char), 1, output);
                i += 2;

                fwrite(&cur_byte, sizeof(unsigned char), 1, output);
                fwrite(argv[3] + begin_value, sizeof(unsigned char),
                       end_value - begin_value, output);
                i += prev_size;
            }
            else {
                fwrite(file_data + i, sizeof(unsigned char), 1, output);
                ++i;
            }
        }
        fclose(output);

        //printf("I AM HERE!\n");
    }

    return 0;
}