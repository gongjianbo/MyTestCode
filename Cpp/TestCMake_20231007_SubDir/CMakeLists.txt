cmake_minimum_required(VERSION 3.10)

project("TestCMake_20231007_SubDir")

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(LIB_VERSION "1.0.0")
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

if(CMAKE_CL_64)
    set(CL_ARCH "64")
else()
    set(CL_ARCH "32")
endif()

if(MSVC)
    # 设置 UTF-8 编码
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D_UNICODE /DUNICODE")
    # 设置 MT/MTd
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

# 内部构建（in source build），即在 CMakeLists.txt 路径（工程顶层目录）执行 cmake .，会在当前目录生成一堆临时文件
# 外部构建（out of source build），即在别的路径执行 cmake 命令，如 CMakeLists.txt 旁新建 build 文件夹，然后执行 cmake ..，缓存会放在 build 文件夹
# CMAKE_BINARY_DIR/PROJECT_BINARY_DIR/<projectname>_BINARY_DIR，构建执行所在的目录，很明显内部构建和外部构建时目录会不同
# CMAKE_SOURCE_DIR/PROJECT_SOURCE_DIR/<projectname>_SOURCE_DIR，始终是工程顶层目录
# CMAKE_CURRENT_SOURCE_DIR，当前 CMakeLists.txt 所在路径，因为 CMake 可以包含子目录，所以这里做区分
# 链接库生成路径
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/lib)
# 可执行程序生成路径
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/build/app)

if(EXISTS ${CMAKE_SOURCE_DIR}/test_lib)
    add_subdirectory(test_lib)
endif()
if(EXISTS ${CMAKE_SOURCE_DIR}/test_app)
    add_subdirectory(test_app)
endif()
