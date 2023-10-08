#ifndef TEST_LIB_H
#define TEST_LIB_H

#ifndef Lib_Api
#  if defined(_MSC_VER) || defined(WIN64) || defined(_WIN64) || defined(__WIN64__) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
#    define Lib_Export __declspec(dllexport)
#    define Lib_Import __declspec(dllimport)
#  else
#    define Lib_Export __attribute__((visibility("default")))
#    define Lib_Import __attribute__((visibility("default")))
#  endif

#  if defined(TEST_LIB_EXPORT)
#    define Lib_Api Lib_Export
#  else
#    define Lib_Api Lib_Import
#  endif
#endif

#ifdef __cplusplus
extern "C"{
#else
#include <stdbool.h>
#endif

Lib_Api int
Lib_Add(int a, int b);

#ifdef __cplusplus
}
#endif

#endif // TEST_LIB_H
