cmake_minimum_required(VERSION 3.10)

project("test_app")

# test_lib 源码存在的时候直接包含，免得还没 copy 文件到依赖目录时找不到
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../test_lib)
    message(STATUS "test_app: Using lib source")
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../test_lib)
else()
    message(STATUS "test_app: Using installed lib")
    # test_lib install 后包含目录
    include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
    link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)
endif()

# 第一个参数是生成文件的名称（TARGET），使用 ${PROJECT_NAME} 引用 project() 项目名对其命名
add_executable(${PROJECT_NAME} main.cpp)

# 链接库
target_link_libraries(${PROJECT_NAME} test_lib-${LIB_VERSION})
