cmake_minimum_required(VERSION 3.10)

project("test_lib")

# 包含目录
# include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)
# link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../lib)

# 生成文件命名结尾
# set(CMAKE_DEBUG_POSTFIX d-${LIB_VERSION})
# set(CMAKE_RELEASE_POSTFIX -${LIB_VERSION})
# set(CMAKE_RELWITHDEBINFO_POSTFIX -${LIB_VERSION})
# set(CMAKE_MINSIZEREL_POSTFIX -${LIB_VERSION})
# 宏定义
add_definitions(-DTEST_LIB_EXPORT)

# 第一个参数是生成文件的名称（TARGET），使用 ${PROJECT_NAME} 引用 project() 项目名对其命名
set(LIB_NAME ${PROJECT_NAME}-${LIB_VERSION})
add_library(${LIB_NAME} SHARED
    test_lib.h
    test_lib.cpp)

# 链接库
# if(MSVC)
#     target_link_libraries(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/../lib/xxx.lib)
# endif()

# install 放到 add_executable/add_library 之后，不然找不到
# 安装命令的路径前缀，也可以用 install 命令的 prefix 参数指定
# set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR}/../)
# 安装生成文件
install(TARGETS ${LIB_NAME} DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../lib)
# 安装项目文件
install(FILES test_lib.h DESTINATION ${CMAKE_CURRENT_SOURCE_DIR}/../include)
