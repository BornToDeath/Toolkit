cmake_minimum_required(VERSION 3.4)
set(PROJECT_NAME ThreadPool)
project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 11)

if (INCLUDE_THREADPOOL)
    return()
endif ()

set(INCLUDE_THREADPOOL YES CACHE STRING DOCSTRING FORCE)
message(STATUS "正在编译 ${PROJECT_NAME}")

# =================== 测试 ===================
function(testThreadPool)
    # 编译成可执行程序
    add_executable(runThreadPool test/test.cpp)
    target_include_directories(runThreadPool PUBLIC Export_Inc)
endfunction()
testThreadPool()
