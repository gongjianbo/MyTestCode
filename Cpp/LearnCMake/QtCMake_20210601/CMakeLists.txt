#CMakeLists.txt文件名区分大小写
#CMake语言不区分大小写，但是参数区分大小写
#所需CMake最低版本，这里设置为3.14
cmake_minimum_required(VERSION 3.14)
#声明项目名称和支持的语言，CMake默认语言为C++
project(AudioTool VERSION 1.0 LANGUAGES CXX)
#设置工程包含当前目录
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

#设置C++标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

#find_file：在相应路径下查找命名文件
#find_library：查找一个库文件
#find_package：从外部项目查找和加载设置
#find_path：查找包含指定文件的目录
#find_program：找到一个可执行程序
#从外部项目中查找和加载设置
#find_package有Module模式和Config模式
#区分Config模式：指定CONFIG关键字/指定NO_MODULE关键字/指定不在MODULE模式支持的关键字
#可能会在COMPONENTS选项之后（或在REQUIRED选项之后（如果存在））列出所需组件的特定于包的列表
#其他可选组件可能会在 之后列出 OPTIONAL_COMPONENTS
#默认情况下，该命令搜索名称为 的包<PackageName>
#如果NAMES给出了该选项，则使用它后面的名称而不是<PackageName>
#CONFIG-Config模式要找<PackageName>Config.cmake或<lower-case-package-name>-config.cmake
find_package(QT NAMES Qt6 Qt5 COMPONENTS Core Quick REQUIRED)
#MODULE-Module模式下是要查找到名为Find<PackageName>.cmake的文件
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Quick REQUIRED)
#未指定的模块将被禁用
#find_package(Qt5 COMPONENTS Qt5Core Qt5Quick REQUIRED)

#定义/设置变量
#普通变量 set(<variable> <value>... [PARENT_SCOPE])
#缓存条目 set(<variable> <value>... CACHE <type> <docstring> [FORCE])
#环境变量 set(ENV{<variable>} [<value>])
#set(PROJECT_SOURCES main.cpp qml.qrc)
#list APPEND效果类似set定义的列表
list(APPEND PROJECT_SOURCES main.cpp qml.qrc MyClass.h)

#显示一条信息
#message([STATUS|WARNING|AUTHOR_WARNING|FATAL_ERROR|SEND_ERROR]
#        "message to display" ...)
#(无) = 重要消息；
#STATUS = 非重要消息；
#WARNING = CMake 警告, 会继续执行；
#AUTHOR_WARNING = CMake 警告 (dev), 会继续执行；
#SEND_ERROR = CMake 错误, 继续执行，但是会跳过生成的步骤；
#FATAL_ERROR = CMake 错误, 终止所有处理过程；
#下面语句输出：CMake message ... main.cpp;qml.qrc
message("CMake message . PROJECT_SOURCES: ${PROJECT_SOURCES}")

#设置编译选项
#PRIVATE，编译选项会应用于给定的目标，不会传递给与目标相关的目标。
#INTERFACE，给定的编译选项将只应用于指定目标，并传递给与目标相关的目标
#PUBLIC，编译选项将应用于指定目标和使用它的目标。
#list(APPEND flags "-xxx" "-yyy")
#target_compile_options(AudioTool PRIVATE ${flags})
#target_compile_options(AudioTool PRIVATE "-zzz")
message("C++ compiler flags: ${CMAKE_CXX_FLAGS}")
message("System name: ${CMAKE_SYSTEM_NAME}")
#检查空指针类型的大小。
#CMake的 CMAKE_SIZEOF_VOID_P 变量会告诉我们CPU是32位还是64位
message("Sizeof void p: ${CMAKE_SIZEOF_VOID_P}")

#使用指定的源文件来生成目标可执行文件
#AudioTool可执行文件；${PROJECT_SOURCES}源文件
add_executable(AudioTool
    ${PROJECT_SOURCES}
    )
#添加源文件
target_sources(AudioTool PRIVATE MyClass.cpp)
#给目标定义预编译宏，貌似得写在add_executable后面
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
    target_compile_definitions(AudioTool PUBLIC "IS_32_BIT_ARCH")
else()
    target_compile_definitions(AudioTool PUBLIC IS_64_BIT_ARCH)
endif()

#生成库
#STATIC静态库；SHARED动态库；OBJECT源码编译到布标文件；MODULE运行时插件
#add_library(LibName STATIC main.cpp)
#链接静态库到可执行文件
#target_link_libraries(AudioTool LibName)

target_compile_definitions(AudioTool
    PRIVATE $<$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>:QT_QML_DEBUG>)
#添加Qt依赖
target_link_libraries(AudioTool
    PRIVATE Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Quick)

#名字里带MACOSX，应该用不到
#set_target_properties(AudioTool PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
#    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
#    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
#    )

if(QT_VERSION_MAJOR EQUAL 6)
    qt_import_qml_plugins(AudioTool)
    qt_finalize_executable(AudioTool)
endif()
#找到一篇Qt4 CMake入门的博客
#https://www.cnblogs.com/JohnShao/archive/2011/09/27/2192834.html
