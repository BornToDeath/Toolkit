cmake_minimum_required(VERSION 3.4.1)
set(PROJECT_NAME HPSocket)
project(${PROJECT_NAME})

#message("_INCLUDE_HPSocket_: ${_INCLUDE_HPSocket_}")

if (_INCLUDE_HPSOCKET_)
    return()
endif ()

set(_INCLUDE_HPSOCKET_ YES CACHE STRING DOCSTRING FORCE)
message(STATUS "正在编译 ${PROJECT_NAME}")

# 添加宏定义否则三方库 HPSocket 编译不通过
ADD_DEFINITIONS(-D_BROTLI_DISABLED)
ADD_DEFINITIONS(-D_SSL_DISABLED)
ADD_DEFINITIONS(-D_HTTP_DISABLED)
ADD_DEFINITIONS(-D_ICONV_DISABLED)
ADD_DEFINITIONS(-D_ZLIB_DISABLED)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-suggest-override")

# 本工程的源文件
file(GLOB_RECURSE HPSOCKET_SRCS ./src/*.cpp ./src/*.c)

# 编译成静态库
add_library(hpsocket_static STATIC ${HPSOCKET_SRCS})

# 引用的头文件目录
target_include_directories(
        hpsocket_static PUBLIC
        include
        src
)

# 测试版本编译成动态库
if (BUILD_SHARED)
    add_library(hpsocket_shared SHARED ${HPSOCKET_SRCS})
endif ()
