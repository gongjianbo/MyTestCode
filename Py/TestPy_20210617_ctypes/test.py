import ctypes

#dll = ctypes.cdll.LoadLibrary("msvcrt.dll")
#dll.printf(b"hello ctypes\n")

dll = ctypes.cdll.LoadLibrary(
    "D:/TestSpace/TestCpp_20210617_mydll/x64/Release/mydll.dll")
#访问变量
print(dll.my_number)  # 打印<_FuncPtr object at 0x0000022537F1B450>，默认应该是当作函数访问的
print(ctypes.c_int.in_dll(dll, 'my_number').value)
#访问函数
print(dll.my_func)
print(dll.my_func(12, 34))
my_func2 = dll.my_func2  # ctypes输入输出默认为int类型
my_func2.argtypes = [ctypes.c_double, ctypes.c_double]
my_func2.restype = ctypes.c_double
print(my_func2(12, 34.5))
my_func3 = dll.my_func3
my_func3.argtypes = [ctypes.c_char_p, ctypes.POINTER(ctypes.c_double)]
my_func3.restype = ctypes.POINTER(ctypes.c_int)
arg1 = ctypes.c_char(12)
arg2 = ctypes.c_double(1000)
#byref相当于取引用，c字符串传字节数组
ret = my_func3(ctypes.byref(arg1), ctypes.byref(arg2), b'hello ctypes.')
print(ret.contents.value)

dll.print_var()
#指针变量
#ptr = ctypes.POINTER(ctypes.c_int)
# char(65)->print(b'A')，可能会把后面挨着的字节内容也打印
print(ctypes.c_char_p.in_dll(dll, 'char_ptr').value)
print(ctypes.POINTER(ctypes.c_int).in_dll(dll, 'int_ptr').contents.value)
ctypes.POINTER(ctypes.c_int).in_dll(dll, 'int_ptr').contents.value = 520
#空指针bool值为false
print(bool(ctypes.POINTER(ctypes.c_int).in_dll(dll, 'null_ptr')))
dll.print_var()

#struct或union必须派生ctypes给的基类Structure和Union
#每个子类都必须定义一个_fields_属性，_fields_必须是一个2-tuples列表，包含一个字段名和一个字段类型。


class MyStruct(ctypes.Structure):
    _fields_ = [
        ("a", ctypes.c_int),
        ("b", ctypes.c_double)
    ]


my_func4 = dll.my_func4
my_func4.restype = MyStruct
arg = MyStruct()
arg.a = 10
arg.b = 11
#print(ctypes.sizeof(MyStruct))
ret = my_func4(arg)
print('my_func4 ret:', ret.a, '  ', ret.b)
my_func5 = dll.my_func5
my_func5.restype = ctypes.POINTER(MyStruct)
ret = my_func5(ctypes.byref(arg)).contents
print('my_func5 ret:', ret.a, '  ', ret.b)
