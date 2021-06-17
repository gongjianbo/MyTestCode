// 下列 ifdef 块是创建使从 DLL 导出更简单的宏的标准方法。
// 在预处理器中定义了 MYDLL_EXPORTS 符号，而调用方不包含此符号。
// 源文件中包含此文件的任何其他项目都会将 MYDLL_API 视为是从 DLL 导入的，
// 而此 DLL 则将用此宏定义的符号视为是被导出的。
#ifdef MYDLL_EXPORTS
#define MYDLL_API __declspec(dllexport)
#else
#define MYDLL_API __declspec(dllimport)
#endif

//ctypes需要提供c接口，主要是目前cpp还没有统一的abi
extern "C"
{
	//导出一个全局变量
	extern MYDLL_API int my_number;
	//函数
	MYDLL_API int my_func(int a, int b);
	MYDLL_API double my_func2(double a, double b);
	MYDLL_API int * my_func3(char *a, double *b, const char *str);
	//指针
	extern MYDLL_API char *char_ptr;
	extern MYDLL_API int *int_ptr;
	extern MYDLL_API int *null_ptr;
	//打印dll中的变量
	MYDLL_API void print_var();
	//结构体
	struct MYDLL_API MyStruct
	{
		int a;
		double b;
	};
	MYDLL_API MyStruct my_func4(const MyStruct &arg);
	MYDLL_API MyStruct * my_func5(MyStruct *arg);
}
