cmake_minimum_required(VERSION 3.4)
set(PROJECT_NAME SharedPreference)
project(${PROJECT_NAME})

if (INCLUDE_SHARED_PREFERENCE)
    return()
endif ()

message(STATUS "正在编译 ${PROJECT_NAME}")
set(INCLUDE_SHARED_PREFERENCE YES CACHE STRING DOCSTRING FORCE)
set(CMAKE_CXX_STANDARD 11)

# 覆盖率统计
option(COVERAGE "set ON to open coverage statistics" OFF)
if (COVERAGE)
    message(STATUS "${PROJECT_NAME} 进行覆盖率统计")
    # 添加覆盖率编译参数
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()

# 库名称
set(LIB_NAME "shared_preference_static")

# 本工程的源文件
aux_source_directory(src SHARED_PREFERENCE_SRCS)

# 编译为库文件
add_library(${LIB_NAME} STATIC ${SHARED_PREFERENCE_SRCS})

# 引用的头文件目录
target_include_directories(
        ${LIB_NAME} PUBLIC
        include
)

# 引用的外部模块
add_subdirectory(../Database Database.out)

# 链接的库
target_link_libraries(
        ${LIB_NAME}
        database_static
)

# 测试
option(SHARED_PREFERENCE_TEST "set ON to run unit test" OFF)
if (SHARED_PREFERENCE_TEST)
    add_compile_options(-g)
    find_package(GTest CONFIG REQUIRED)

    aux_source_directory(test TEST_SRCS)
    set(TEST_BINARY_NAME run_shared_preference_unittest)
    add_executable(${TEST_BINARY_NAME} ${TEST_SRCS})
    target_link_libraries(${TEST_BINARY_NAME}
            ${LIB_NAME}
            GTest::gtest_main
    )
endif ()