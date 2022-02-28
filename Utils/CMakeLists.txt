cmake_minimum_required(VERSION 3.4)
set(PROJECT_NAME Utils)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 11)

if (NOT INCLUDE_UTILS)
    set(INCLUDE_UTILS YES CACHE STRING DOCSTRING FORCE)
    message(STATUS "正在编译 ${PROJECT_NAME}")

    # 覆盖率统计
    if (COVERAGE)
        message(STATUS "${PROJECT_NAME} 进行覆盖率统计")
        # 添加覆盖率编译参数
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    endif ()

    # 源文件目录
    file(GLOB_RECURSE UTILS_SRCS Source/*.cpp)

    # 编译为库文件
    add_library(Utils_static STATIC ${UTILS_SRCS})

    # 头文件引用目录
    target_include_directories(
            Utils_static PUBLIC
            Export_Inc
    )

    # =================== 测试 ===================
    function(testUtils)
        # 编译成可执行程序
        add_executable(runUtils test/test.cpp)
        target_link_libraries(runUtils Utils_static)
    endfunction()
#    testUtils()

endif ()
