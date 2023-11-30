cmake_minimum_required(VERSION 3.4)
set(PROJECT_NAME ThreadPool)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 11)

if (INCLUDE_THREAD_POOL)
    return()
endif ()

set(INCLUDE_THREAD_POOL YES CACHE STRING DOCSTRING FORCE)
message(STATUS "正在编译 ${PROJECT_NAME}")

option(COVERAGE "set ON to open coverage statistics" OFF)
if (COVERAGE)
    message(STATUS "${PROJECT_NAME} 进行覆盖率统计")
    # 添加覆盖率编译参数
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()

# 测试
option(THREAD_POOL_TEST "set ON to run thread pool unit test" OFF)
if (THREAD_POOL_TEST)
    add_compile_options(-g)
    find_package(GTest CONFIG REQUIRED)

    aux_source_directory(test TEST_SRCS)
    set(TEST_BINARY_NAME run_thread_pool_unittest)
    add_executable(${TEST_BINARY_NAME} ${TEST_SRCS})
    target_include_directories(${TEST_BINARY_NAME} PUBLIC
            include
    )
    target_link_libraries(${TEST_BINARY_NAME}
            pthread
            GTest::gtest_main
    )
endif ()