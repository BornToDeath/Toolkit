cmake_minimum_required(VERSION 3.4)
set(PROJECT_NAME ThreadDispatcher)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 11)

if (NOT INCLUDE_THREAD_DISPATCHER)
    set(INCLUDE_THREAD_DISPATCHER YES CACHE STRING DOCSTRING FORCE)
    message(STATUS "正在编译 ${PROJECT_NAME}")

    # 覆盖率统计
    if (COVERAGE)
        message(STATUS "${PROJECT_NAME} 进行覆盖率统计")
        # 添加覆盖率编译参数
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    endif ()

    # 本工程的源文件
    aux_source_directory(Source THREAD_DISPATCHER_SRCS)

    # 编译为库文件
    add_library(ThreadDispatcher_static STATIC ${THREAD_DISPATCHER_SRCS})

    # 引用的头文件目录
    target_include_directories(
            ThreadDispatcher_static PUBLIC
            Export_Inc
    )

    # 引用的外部模块
    add_subdirectory(../Thread Thread.out)

    # 链接库
    target_link_libraries(
            ThreadDispatcher_static
            Thread_static
    )

    # 测试
    if (UNIT_TEST)
        add_executable(runThreadDispatcher test/test.cpp)
        target_link_libraries(
                runThreadDispatcher
                ThreadDispatcher_static
                pthread
        )
    endif ()

endif ()