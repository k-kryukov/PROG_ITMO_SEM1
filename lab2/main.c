#include "uint1024_t.h"

int main(void) {
	uint1024_t x, y, z, w, v;
	x = from_str("9999999999999");
	y = from_str("2784373894782984");
	
	z = add_op(x, y);
	w = subtr_op(y, x);
	v = mult_op(x, y);
	
	printf_value(x);
	printf(" + ");
	printf_value(y);
	printf(" = ");
	printf_value(z);
	printf("\n");
	
	printf_value(y);
	printf(" - ");
	printf_value(x);
	printf(" = ");
	printf_value(w);
	printf("\n");
	
	printf_value(x);
	printf(" * ");
	printf_value(y);
	printf(" = ");
	printf_value(v);	
	printf("\n");
	
	system("pause");
	return 0;
}