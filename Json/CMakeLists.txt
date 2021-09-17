cmake_minimum_required(VERSION 3.4)
set(PROJECT_NAME Json)
project(${PROJECT_NAME})

if (NOT INCLUDE_JSON)
    message(STATUS "正在编译 ${PROJECT_NAME}")
    set(INCLUDE_JSON YES CACHE STRING DOCSTRING FORCE)
    set(CMAKE_CXX_STANDARD 11)

    # 覆盖率统计
    if (COVERAGE)
        message(STATUS "${PROJECT_NAME} 进行覆盖率统计")
        # 添加覆盖率编译参数
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    endif ()

    # 库名称
    set(LIB_NAME "Json_static")

    # 本工程的源文件
    file(GLOB_RECURSE JSON_SRCS Export_Inc/*.cpp)

    # 编译为库文件
    add_library(${LIB_NAME} STATIC ${JSON_SRCS})

    # 引用的头文件目录
    target_include_directories(
            ${LIB_NAME} PUBLIC
            Export_Inc
    )

    # 测试
    set(UNIT_TEST YES)
    if (UNIT_TEST)
        add_executable(runJson test/test.cpp)
        target_link_libraries(
                runJson
                ${LIB_NAME}
        )
    endif ()
endif ()