cmake_minimum_required(VERSION 3.4)
set(PROJECT_NAME ThreadDispatcher)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 11)

if (INCLUDE_THREAD_DISPATCHER)
    return()
endif ()

set(INCLUDE_THREAD_DISPATCHER YES CACHE STRING DOCSTRING FORCE)
message(STATUS "正在编译 ${PROJECT_NAME}")

# 覆盖率统计
option(COVERAGE "set ON to open coverage statistics" OFF)
if (COVERAGE)
    message(STATUS "${PROJECT_NAME} 进行覆盖率统计")
    # 添加覆盖率编译参数
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()

# 本工程的源文件
aux_source_directory(src THREAD_DISPATCHER_SRCS)

# 编译为库文件
set(LIB_NAME "thread_dispatcher_static")
add_library(${LIB_NAME} STATIC ${THREAD_DISPATCHER_SRCS})

# 引用的头文件目录
target_include_directories(${LIB_NAME} PUBLIC
        include
)

# 引用的外部模块
add_subdirectory(../Thread Thread.out)

# 链接库
target_link_libraries(${LIB_NAME}
        thread_static
)

# 测试
option(THREAD_DISPATCHER_TEST "set ON to run unit test" OFF)
if (THREAD_DISPATCHER_TEST)
    add_compile_options(-g)
    find_package(GTest CONFIG REQUIRED)

    aux_source_directory(test TEST_SRCS)
    set(TEST_BINARY_NAME run_thread_dispatcher_unittest)
    add_executable(${TEST_BINARY_NAME} ${TEST_SRCS})
    target_link_libraries(${TEST_BINARY_NAME}
            ${LIB_NAME}
            GTest::gtest_main
            pthread
    )
endif ()