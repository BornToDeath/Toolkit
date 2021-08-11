cmake_minimum_required(VERSION 3.4)
set(PROJECT_NAME Log)
project(${PROJECT_NAME})

if (NOT INCLUDE_LOG)
    message(STATUS "正在编译 ${PROJECT_NAME}")
    set(INCLUDE_LOG YES CACHE STRING DOCSTRING FORCE)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

    # 单元测试
    if (UNIT_TEST)
        message(STATUS "${PROJECT_NAME} 进行覆盖率统计")
        # 添加覆盖率编译参数
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    endif ()

    aux_source_directory(Source LOG_SRCS)
    add_library(Log_static STATIC ${LOG_SRCS})
    target_include_directories(
            Log_static PUBLIC
            Export_Inc
    )

    # 测试
    if (LOG_TEST)
        add_definitions(-DBUILD_TYPE_DEBUG)
        add_executable(runLog test/test.cpp)
        target_link_libraries(
                runLog
                Log_static
        )
    endif ()

endif ()