cmake_minimum_required(VERSION 3.4)
set(PROJECT_NAME Log)
project(${PROJECT_NAME})

if (INCLUDE_LOG)
    return()
endif ()

message(STATUS "正在编译 ${PROJECT_NAME}")
set(INCLUDE_LOG YES CACHE STRING DOCSTRING FORCE)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
#    add_compile_options(-fPIC)

if (CMAKE_BUILD_TYPE MATCHES "DEBUG")
    message(STATUS "add macro BUILD_TYPE_DEBUG")
    ADD_DEFINITIONS(-DBUILD_TYPE_DEBUG)
endif ()

aux_source_directory(src LOG_SRCS)
add_library(log_static STATIC ${LOG_SRCS})
target_include_directories(
        log_static PUBLIC
        include
)

# 覆盖率统计
option(COVERAGE "set ON to open coverage statistics" OFF)
if (COVERAGE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()

# -------------------- 单元测试 --------------------
option(LOG_TEST "set ON to run log unit test" OFF)
if (LOG_TEST)
    message(STATUS "unit test: log")
    add_compile_options(-g)
    find_package(GTest CONFIG REQUIRED)

    # 编译成可执行程序
    aux_source_directory(test LOG_TEST_SRCS)
    set(TEST_BINARY_NAME run_log_unittest)
    add_executable(${TEST_BINARY_NAME} ${LOG_TEST_SRCS})
    target_link_libraries(${TEST_BINARY_NAME}
            log_static
            pthread
            GTest::gtest_main
    )
endif ()