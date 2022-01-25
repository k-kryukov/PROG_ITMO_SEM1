#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

extern const int64_t BASE;

#define DIGIT_CNT 32

extern const unsigned int MX_BYTE;
// MAX NUMBER WHICH CAN BE MOVED LEFT ON ONE BIT
// WITHOUT OVERFLOW

enum {
	BIGGER,
	EQUAL,
	LOWER
};

typedef struct {
	uint32_t a[DIGIT_CNT];
} uint1024_t;

uint1024_t subtr_op(uint1024_t x, uint1024_t y);
void printf_value_hex(uint1024_t x);
void printf_value(uint1024_t x);
uint1024_t mult_op(uint1024_t x, uint1024_t y);
int digitals_sum(uint1024_t x);
void bit_move_left(uint1024_t* x);
void scanf_value(uint1024_t* x);
uint1024_t from_str(char* t);
void make_null(uint1024_t* a);
uint1024_t divide_ten(uint1024_t x);
bool positive(uint1024_t a);
unsigned char comp(uint1024_t a, uint1024_t b);
uint1024_t mult_int(uint1024_t a, int b);
uint1024_t add_op(uint1024_t a, uint1024_t b);
uint1024_t from_uint(uint32_t a);