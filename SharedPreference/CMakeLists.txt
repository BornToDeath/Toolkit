cmake_minimum_required(VERSION 3.4)
set(PROJECT_NAME SharedPreference)
project(${PROJECT_NAME})

if (NOT INCLUDE_SHAREDPREFERENCE)
    message(STATUS "正在编译 ${PROJECT_NAME}")
    set(INCLUDE_SHAREDPREFERENCE YES CACHE STRING DOCSTRING FORCE)
    set(CMAKE_CXX_STANDARD 11)

    # 覆盖率统计
    if (COVERAGE)
        message(STATUS "${PROJECT_NAME} 进行覆盖率统计")
        # 添加覆盖率编译参数
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    endif ()

    # 库名称
    set(LIB_NAME "SharedPreference_static")

    # 本工程的源文件
    aux_source_directory(Source SHAREDPREFERENCE_SRCS)

    # 编译为库文件
    add_library(${LIB_NAME} STATIC ${SHAREDPREFERENCE_SRCS})

    # 引用的头文件目录
    target_include_directories(
            ${LIB_NAME} PUBLIC
            Export_Inc
    )

    # 引用的外部模块
    add_subdirectory(../Database Database.out)

    # 链接的库
    target_link_libraries(
            ${LIB_NAME}
            Database_static
    )

    # 测试
    set(UNIT_TEST YES)
    if (UNIT_TEST)
        add_executable(runSharedPreference test/test.cpp)
        target_link_libraries(
                runSharedPreference
                ${LIB_NAME}
        )
    endif ()
endif()