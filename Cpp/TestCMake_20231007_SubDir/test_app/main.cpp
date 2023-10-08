#include <iostream>
#include "test_lib.h"

int main()
{
	int a = 10;
	int b = 20;
    int c = Lib_Add(a, b);
	fprintf(stderr, "Lib_Add(%d, %d) = %d\n", a, b, c);
	return 0;
}
