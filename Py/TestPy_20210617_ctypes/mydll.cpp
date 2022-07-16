// MSVC模板默认带pch，设置为不使用预编译头
// #include "pch.h"
#include "mydll.h"
#include <iostream>

MYDLL_API int my_number = 1992;

MYDLL_API int my_func(int a, int b)
{
	return a + b;
}

MYDLL_API double my_func2(double a, double b)
{
	return a + b;
}

MYDLL_API int * my_func3(char * a, double * b, const char *str)
{
	std::cout << "dll myfunc3:" << str << std::endl;
	my_number = *a + int(*b);
	return &my_number;
}

MYDLL_API char *char_ptr = new char(65);
MYDLL_API int *int_ptr = new int(250);
MYDLL_API int *null_ptr = nullptr;

MYDLL_API void print_var()
{
	std::cout << "dll print var:"
		<< "\n\tmy number:" << my_number
		<< "\n\tchar ptr:" << *char_ptr
		<< "\n\tint ptr:" << *int_ptr << std::endl;
}

MYDLL_API MyStruct my_func4(const MyStruct & arg)
{
	MyStruct ret{ 12,34.5 };
	std::cout << "dll myfunc4:"
		<< "in:" << arg.a << " " << arg.b
		<< "ret:" << ret.a << " " << ret.b << std::endl;
	return ret;
}

MYDLL_API MyStruct * my_func5(MyStruct * arg)
{
	if (arg) {
		arg->a = 67;
		arg->b = 89.5;
	}
	return arg;
}
