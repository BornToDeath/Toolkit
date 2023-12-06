cmake_minimum_required(VERSION 3.4)
set(PROJECT_NAME NotificationCenter)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 11)

if (INCLUDE_NOTIFICATION_CENTER)
    return()
endif ()

set(INCLUDE_NOTIFICATION_CENTER YES CACHE STRING DOCSTRING FORCE)
message(STATUS "正在编译 ${PROJECT_NAME}")

# 覆盖率统计
option(COVERAGE "set ON to open coverage statistics" OFF)
if (COVERAGE)
    message(STATUS "${PROJECT_NAME} 进行覆盖率统计")
    # 添加覆盖率编译参数
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
endif ()

# 本工程的源文件
aux_source_directory(src NOTIFICATION_CENTER_SRCS)

# 编译为库文件
add_library(notification_center_static STATIC ${NOTIFICATION_CENTER_SRCS})

# 引用的头文件目录
target_include_directories(
        notification_center_static PUBLIC
        include
)

# 引用的外部模块
add_subdirectory(../Thread Thread.out)

# 链接库
target_link_libraries(
        notification_center_static
        thread_static
)

# 测试
option(NOTIFICATION_CENTER_TEST "set ON to run notification center unit test" OFF)
if (NOTIFICATION_CENTER_TEST)
    add_compile_options(-g)
    find_package(GTest CONFIG REQUIRED)

    aux_source_directory(test TEST_SRCS)
    set(TEST_BINARY_NAME run_notification_center_unittest)
    add_executable(${TEST_BINARY_NAME} ${TEST_SRCS})
    target_link_libraries(${TEST_BINARY_NAME}
            notification_center_static
            pthread
            GTest::gtest_main
    )
endif ()