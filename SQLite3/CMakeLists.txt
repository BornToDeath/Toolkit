cmake_minimum_required(VERSION 3.4)
set(PROJECT_NAME SQLite3)
project(${PROJECT_NAME})

if (NOT INCLUDE_SQLITE3)
    message(STATUS "正在编译 ${PROJECT_NAME}")
    set(INCLUDE_SQLITE3 YES CACHE STRING DOCSTRING FORCE)
    set(CMAKE_CXX_STANDARD 11)

    # 覆盖率统计
    if (COVERAGE)
        message(STATUS "${PROJECT_NAME} 进行覆盖率统计")
        # 添加覆盖率编译参数
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    endif ()

    # 库名称
    set(LIB_NAME "Sqlite3_static")

    # 本工程的源文件
    file(GLOB_RECURSE SQLITE3_SRCS Source/*.cpp Source/*.c)

    # 编译为库文件
    add_library(${LIB_NAME} STATIC ${SQLITE3_SRCS})

    # 引用的头文件目录
    target_include_directories(
            ${LIB_NAME} PUBLIC
            Export_Inc
            Source
    )

    # 链接的库文件
    target_link_libraries(
            ${LIB_NAME}
            pthread
            dl
    )

    # 测试
    if (UNIT_TEST)
        add_executable(runSqlite3 test/test.cpp)
        target_link_libraries(
                runSqlite3
                ${LIB_NAME}
                pthread
                dl
        )
    endif ()

endif ()