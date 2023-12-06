cmake_minimum_required(VERSION 3.4.1)
set(PROJECT_NAME Communication)
project(${PROJECT_NAME})

# 如果没有包含Communication模块
if (INCLUDE_COMMUNICATION)
    return()
endif ()

message(STATUS "正在编译 ${PROJECT_NAME}")
set(INCLUDE_COMMUNICATION YES CACHE STRING DOCSTRING FORCE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
set(CMAKE_CXX_STANDARD 14)

if (CMAKE_BUILD_TYPE MATCHES "DEBUG")
    add_compile_options(-g)
endif ()

# 覆盖率统计
option(COVERAGE "set ON to open coverage statistics" OFF)
if (COVERAGE)
    message(STATUS "${PROJECT_NAME} 进行覆盖率统计")
    # 添加覆盖率编译参数
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()

# 本工程的源文件
file(GLOB_RECURSE COMMUNICATION_SRCS client/src/*.cpp server/src/*.cpp)

# 编译成静态库
add_library(communication_static STATIC ${COMMUNICATION_SRCS})

# 引用的头文件目录
target_include_directories(
        communication_static PUBLIC
        client/include
        client/src
        server/include
        hpsocket/include
        ../Log/include
        ../NotificationCenter/headers
        ../Utils/include
        ../Thread/include
)

# 引用的外部模块
add_subdirectory(hpsocket hpsocket.out)
add_subdirectory(../Log Log.out)
add_subdirectory(../NotificationCenter NotificationCenter.out)
add_subdirectory(../Thread Thread.out)
add_subdirectory(../Utils Utils.out)

# 链接库
target_link_libraries(
        communication_static
        -Wl,--start-group
        hpsocket_static
        notification_center_static
        thread_static
        utils_static
        log_static
        -Wl,--end-group
)

option(COMMUNICATION_TEST "set ON to run unit test" OFF)
if (COMMUNICATION_TEST)
    add_compile_options(-g)
    find_package(GTest CONFIG REQUIRED)

    aux_source_directory(test TEST_SRCS)
    set(TEST_BINARY_NAME run_communication_unittest)
    add_executable(${TEST_BINARY_NAME} ${TEST_SRCS})
    target_link_libraries(${TEST_BINARY_NAME}
            communication_static
            pthread
            GTest::gtest_main
    )
endif ()

