cmake_minimum_required(VERSION 3.4)
set(PROJECT_NAME Zip)
project(${PROJECT_NAME})

if (NOT INCLUDE_ZIP)
    message(STATUS "正在编译 ${PROJECT_NAME}")
    set(INCLUDE_ZIP YES CACHE STRING DOCSTRING FORCE)
    set(CMAKE_CXX_STANDARD 11)

    # 覆盖率统计
    if (COVERAGE)
        message(STATUS "${PROJECT_NAME} 进行覆盖率统计")
        # 添加覆盖率编译参数
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    endif ()

    # 库名称
    set(LIB_NAME "Zip_static")

    # 本工程的源文件
    file(GLOB_RECURSE ZIP_SRCS Export_Inc/*.c)

    # 编译为库文件
    add_library(${LIB_NAME} STATIC ${ZIP_SRCS})

    # 引用的头文件目录
    target_include_directories(
            ${LIB_NAME} PUBLIC
            Export_Inc
    )

    # 测试
    if (UNIT_TEST)
        add_executable(runZip test/test.cpp)
        target_link_libraries(
                runZip
                ${LIB_NAME}
        )
    endif ()
endif ()