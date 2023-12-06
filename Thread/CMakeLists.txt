cmake_minimum_required(VERSION 3.4.1)
set(PROJECT_NAME Thread)
project(${PROJECT_NAME})

if (INCLUDE_THREAD)
    return()
endif ()

message(STATUS "正在编译 ${PROJECT_NAME}")
set(INCLUDE_THREAD YES CACHE STRING DOCSTRING FORCE)
set(CMAKE_CXX_STANDARD 11)

option(COVERAGE "set ON to open coverage statistics" OFF)
if (COVERAGE)
    message(STATUS "${PROJECT_NAME} 进行覆盖率统计")
    # 添加覆盖率编译参数
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()

# 本工程的源文件
file(GLOB_RECURSE THREAD_SRCS src/*.cpp)

# 编译成静态库
add_library(thread_static STATIC ${THREAD_SRCS})

# 引用的头文件目录
target_include_directories(
        thread_static PUBLIC
        include
)

# 测试
option(THREAD_TEST "set ON to run thread unit test" OFF)
if (THREAD_TEST)
    add_compile_options(-g)
    find_package(GTest CONFIG REQUIRED)

    aux_source_directory(test TEST_SRCS)
    set(TEST_BINARY_NAME run_thread_unittest)
    add_executable(${TEST_BINARY_NAME} ${TEST_SRCS})
    target_link_libraries(${TEST_BINARY_NAME}
            thread_static
            pthread
            GTest::gtest_main
    )
endif ()