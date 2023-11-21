cmake_minimum_required(VERSION 3.4)
set(PROJECT_NAME Utils)
project(${PROJECT_NAME})

if (INCLUDE_UTILS)
    return()
endif ()

set(INCLUDE_UTILS YES CACHE STRING DOCSTRING FORCE)
message(STATUS "正在编译 ${PROJECT_NAME}")
set(CMAKE_CXX_STANDARD 11)

# 源文件目录
file(GLOB_RECURSE UTILS_SRCS src/*.cpp)

# 编译为库文件
add_library(utils_static STATIC ${UTILS_SRCS})

# 头文件引用目录
target_include_directories(
        utils_static PUBLIC
        include
)

# 覆盖率统计
option(COVERAGE "set ON to open coverage statistics" OFF)
if (COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()

# -------------------- 单元测试 --------------------
option(UTILS_TEST "set ON to run utils unit test" OFF)
if (UTILS_TEST)
    message(STATUS "unit test: utils")
    add_compile_options(-g)
    find_package(GTest CONFIG REQUIRED)

    # 编译成可执行程序
    aux_source_directory(test TEST_SRCS)
    set(TEST_BINARY_NAME run_utils_unittest)
    add_executable(${TEST_BINARY_NAME} ${TEST_SRCS})
    target_link_libraries(${TEST_BINARY_NAME}
            utils_static
            pthread
            GTest::gtest_main
    )
endif ()