cmake_minimum_required(VERSION 3.4.1)
set(PROJECT_NAME Communication)
project(${PROJECT_NAME})

# 如果没有包含Communication模块
if (NOT INCLUDE_COMMUNICATION)
    message(STATUS "正在编译 ${PROJECT_NAME}")
    set(INCLUDE_COMMUNICATION YES CACHE STRING DOCSTRING FORCE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

    # 覆盖率统计
    if (COVERAGE)
        message(STATUS "${PROJECT_NAME} 进行覆盖率统计")
        # 添加覆盖率编译参数
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
    endif ()

    # 本工程的源文件
    file(GLOB_RECURSE COMMUNICATION_SRCS Client/Source/*.cpp)

    # 编译成静态库
    add_library(Communication_static STATIC ${COMMUNICATION_SRCS})

    # 引用的头文件目录
    target_include_directories(
            Communication_static PUBLIC
            Client/Export_Inc
            Client/Source
            HPSocket/include
            ../Log/Export_Inc
            ../NotificationCenter/headers
            ../Utils/Export_Inc
            ../Thread/Export_Inc
    )

    # 引用的外部模块
    add_subdirectory(HPSocket HPSocket.out)
    add_subdirectory(../Log Log.out)
    add_subdirectory(../NotificationCenter NotificationCenter.out)
    add_subdirectory(../Thread Thread.out)
    add_subdirectory(../Utils Utils.out)

    # 链接库
    target_link_libraries(
            Communication_static
            -Wl,--start-group
            HPSocket_static
            NotificationCenter_static
            Thread_static
            Utils_static
            Log_static
            -Wl,--end-group
    )

    # ********************* 测试 *********************
    function(buildTest)
        add_compile_options(-g)
        add_executable(runCommunication test/test.cpp)
        target_link_libraries(
                runCommunication
                Communication_static
        )
    endfunction()
#    buildTest()

endif ()

