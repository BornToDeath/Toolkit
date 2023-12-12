cmake_minimum_required(VERSION 3.4.1)
set(PROJECT_NAME ZipWrapper)
project(${PROJECT_NAME})

if (INCLUDE_ZIP_WRAPPER)
    return()
endif ()

message(STATUS "正在编译 ${PROJECT_NAME}")
set(INCLUDE_ZIP_WRAPPER YES CACHE STRING DOCSTRING FORCE)
set(CMAKE_CXX_STANDARD 14)

# 覆盖率统计
option(COVERAGE "set ON to open coverage statistics" OFF)
if (COVERAGE)
    message(STATUS "${PROJECT_NAME} 进行覆盖率统计")
    # 添加覆盖率编译参数
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()

# 本工程的源文件
file(GLOB_RECURSE ZIP_SRCS src/*.c src/*.cpp)

# 编译成静态库
set(ZIP_WRAPPER_LIB_NAME zip_wrapper_static)
add_library(${ZIP_WRAPPER_LIB_NAME} STATIC ${ZIP_SRCS})

# 本工程头文件引用路径
target_include_directories(${ZIP_WRAPPER_LIB_NAME} PUBLIC
        include
        src/zip
)

# 测试
option(ZIP_WRAPPER_TEST "set ON to run unit test" OFF)
if (ZIP_WRAPPER_TEST)
    add_compile_options(-g)
    find_package(GTest CONFIG REQUIRED)

    aux_source_directory(test TEST_SRCS)
    set(TEST_BINARY_NAME run_zip_wrapper_unittest)
    add_executable(${TEST_BINARY_NAME} ${TEST_SRCS})
    target_link_libraries(${TEST_BINARY_NAME}
            ${ZIP_WRAPPER_LIB_NAME}
            GTest::gtest_main
    )
endif ()