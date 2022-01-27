#include "uint1024_t.h"

const int64_t BASE = 4294967296;

const unsigned int MX_BYTE = 2147483647u;

uint1024_t from_uint(uint32_t a) {
	uint1024_t res;
	make_null(&res);
	res.a[DIGIT_CNT - 1] = a;
	
	return res;
}

uint1024_t add_op(uint1024_t a, uint1024_t b) {
	uint1024_t result;
	make_null(&result);
	uint64_t over = 0;
	
	for (int i = DIGIT_CNT - 1; i >= 0; --i) {
		uint64_t f = (uint64_t)a.a[i], s = (uint64_t)b.a[i];
		uint64_t res = f + s + over;
		over = (res >> DIGIT_CNT);
		result.a[i] = (uint32_t)res;
	}
	
	if (over != 0) // overflow => return 0
		return from_uint(0);
	return result;
}

uint1024_t mult_int(uint1024_t a, int b) {
	uint1024_t res;
	make_null(&res);
	
	for (int i = 0; i < b; i++)
		res = add_op(res, a);
	
	return res;
}

unsigned char comp(uint1024_t a, uint1024_t b) {
	for (int i = 0; i < DIGIT_CNT; i++) {
		if (a.a[i] > b.a[i])
			return BIGGER;
		else if (a.a[i] < b.a[i])
			return LOWER;
	}
	
	return EQUAL;
}

bool positive(uint1024_t a) {
	for (int i = DIGIT_CNT - 1; i >= 0; --i) {
		if (a.a[i] > 0)
			return  true;
	}
	
	return false;
}

uint1024_t divide_ten(uint1024_t x) {
	int toNext = 0;
	uint1024_t res;
	
	for (int i = 0; i < DIGIT_CNT; i++) {
		if (x.a[i] == 0) {
			res.a[i] = 0;
			continue;
		}
		
		uint64_t temp = ((uint64_t)x.a[i] + (uint64_t)toNext * BASE);
		res.a[i] = (temp / 10);
		toNext = (temp % 10);
	}
	return res;
}

void make_null(uint1024_t* a) {
	for (int i = 0; i < DIGIT_CNT; ++i)
		a->a[i] = 0;
}

uint1024_t from_str(char* t) {
	int n = strlen(t);
	uint1024_t res;
	make_null(&res);
	
	uint1024_t exp = from_uint(1);
	for (int i = n - 1; i >= 0; --i) {
		res = add_op(res, mult_int(exp, t[i] - '0'));
		exp = mult_int(exp, 10);
	}
	
	return res;
}

void scanf_value(uint1024_t* x) {
	char* s = (char*)malloc(sizeof(char) * 400);
	make_null(x);
	scanf("%s", s);
	int n = strlen(s);
	
	for (int i = 0; i < n; i++) {
		char c = s[i];
		*x = add_op(mult_int(*x, 10), from_uint(c - '0'));
	}
}

void bit_move_left(uint1024_t* x) {
	bool move = false;
	
	for (int i = DIGIT_CNT - 1; i >= 0; --i) {
		uint32_t temp = x->a[i];
		bool prevMove = move;
		
		if (temp >= (MX_BYTE + 1u)) {
			move = true;
			if (i == 0)
				*x = from_str("0");
		}
		else
			move = false;
		
		temp = temp << 1;
		x->a[i] = temp + prevMove;
	}
}

int digitals_sum(uint1024_t x) {
	int cnt = 0;
	
	for (int i = 0; i < DIGIT_CNT; i++) {
		int buffer = x.a[i];
		
		for (int j = 0; j < DIGIT_CNT; j++) {
			if (buffer & 1)
				++cnt;
			buffer >>= 1;
		}
	}
	
	return cnt;
}

uint1024_t mult_op(uint1024_t x, uint1024_t y) {
	uint1024_t zero = { 0 };
	if (!positive(x) || !positive(y)) // O(1024)
		return zero;
	
	int cntx = digitals_sum(x); // O(1024)
	int cnty = digitals_sum(y); // O(1024)
	bool useX = false;
	useX = cntx < cnty;
	uint1024_t res;
	make_null(&res);
	int cnt = 0;
	uint32_t buffer;
	
	for (int i = DIGIT_CNT - 1; i >= 0; --i) { // O(32)
		buffer = (useX ? x.a[i] : y.a[i]);
	
		for (int j = DIGIT_CNT - 1; j >= 0; --j) { // O(32)
			if (buffer & 1) {
				for (int k = 0; k < (DIGIT_CNT - 1 - i) * DIGIT_CNT + (DIGIT_CNT - 1 - j) - cnt; ++k) {
					if (useX)
						bit_move_left(&y);
					else
						bit_move_left(&x); // O(1024)
				}
				
				cnt = (DIGIT_CNT - 1 - i) * DIGIT_CNT + (DIGIT_CNT - 1 - j);
				res = useX ? add_op(res, y) : add_op(res, x); // O(1024)
			}
			
			buffer >>= 1;
		}
	}
	
	return res;
}

void printf_value(uint1024_t x) { // число записано в прямом порядке 
	unsigned char buffer[310];
	int counter = 0;
	int toNext = 0;
	
	while (positive(x)) {
		uint1024_t res;
		
		for (int i = 0; i < DIGIT_CNT; i++) {
			int64_t temp = ((int64_t)x.a[i] + (int64_t)toNext * BASE);
			res.a[i] = (temp / 10);
			toNext = temp % 10;
		}
		
		x = res;
		buffer[counter] = toNext;
		toNext = 0;
		counter++;
	}
	if (!counter) {
		printf("0");
		return;
	}
	for (int i = counter - 1; i >= 0; --i)
		printf("%d", buffer[i]);
}

void printf_value_hex(uint1024_t x) {
	bool flag = false;
	
	for (int i = 0; i < DIGIT_CNT; ++i) {
		if (x.a[i] == 0 && !flag) {
			continue;
		}
		flag = true;
		
		if (!x.a[i])
			printf("00000000");
		else
			printf("%x", x.a[i]);
	}
}

uint1024_t subtr_op(uint1024_t x, uint1024_t y) {
	bool need_from_older = false;
	uint1024_t result;
	make_null(&result);
	
	for (int i = DIGIT_CNT - 1; i >= 0; --i) {
		int64_t interm_res = (int64_t)x.a[i] - (int64_t)y.a[i] - (int64_t)need_from_older;
		
		if (interm_res < 0) {
			if (i == 0)
				return from_uint(0);
		
			need_from_older = true;
			result.a[i] = BASE + interm_res;
		}
		else {
			result.a[i] = interm_res;
			need_from_older = false;
		}
	}
	
	return result;
}