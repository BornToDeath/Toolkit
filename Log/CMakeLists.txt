cmake_minimum_required(VERSION 3.4)
set(PROJECT_NAME Log)
project(${PROJECT_NAME})

if (NOT INCLUDE_LOG)
    message(STATUS "正在编译 ${PROJECT_NAME}")
    set(INCLUDE_LOG YES CACHE STRING DOCSTRING FORCE)
    set(CMAKE_CXX_STANDARD 11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
    #    add_compile_options(-fPIC)

    if (CMAKE_BUILD_TYPE MATCHES "DEBUG")
        ADD_DEFINITIONS(-DBUILD_TYPE_DEBUG)
    endif ()

    aux_source_directory(Source LOG_SRCS)
    add_library(Log_static STATIC ${LOG_SRCS})
    target_include_directories(
            Log_static PUBLIC
            Export_Inc
    )

    # -------------------- 测试 --------------------
    function(testLog)
        add_compile_options(-g)
        # 编译成可执行程序
        add_executable(runLog test/test.cpp)
        target_link_libraries(runLog Log_static)
    endfunction()
    #    testLog()

endif ()