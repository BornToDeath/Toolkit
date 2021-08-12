cmake_minimum_required(VERSION 3.4.1)
set(PROJECT_NAME Thread)
project(${PROJECT_NAME})

if (NOT INCLUDE_THREAD)
    message(STATUS "正在编译 ${PROJECT_NAME}")
    set(INCLUDE_THREAD YES CACHE STRING DOCSTRING FORCE)
    set(CMAKE_CXX_STANDARD 11)

    if (UNIT_TEST)
        message(STATUS "${PROJECT_NAME} 进行覆盖率统计")
        # 添加覆盖率编译参数
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    endif ()

    # 本工程的源文件
    file(GLOB_RECURSE THREAD_SRCS Source/*.cpp)

    # 编译成静态库
    add_library(Thread_static STATIC ${THREAD_SRCS})

    # 引用的头文件目录
    target_include_directories(
            Thread_static PUBLIC
            Export_Inc
    )

    # 测试
    set(THREAD_TEST NO)
    if (THREAD_TEST)
        add_executable(runThread test/test.cpp)
        target_link_libraries(
                runThread
                Thread_static
                pthread
        )
    endif ()

endif ()