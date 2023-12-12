cmake_minimum_required(VERSION 3.4)
set(PROJECT_NAME Protobuf)
project(${PROJECT_NAME})

if (INCLUDE_PROTOBUF)
    return()
endif ()

message(STATUS "正在编译 ${PROJECT_NAME}")
set(INCLUDE_PROTOBUF YES CACHE STRING DOCSTRING FORCE)
set(CMAKE_CXX_STANDARD 11)
add_compile_options(-g)

################### 一些开关 ###################

# 1. 使用本机 proto 生成 .pb.h/cc
option(ENABLE_PROTOC_WAY "set ON to enable protoc bin to generate .pb.h/cc" OFF)

# 2. 使用 cmake 方法 protobuf_generate_cpp() 生成 .pb.h/cc
option(ENABLE_CMAKE_WAY "set ON to enable cmake protobuf_generate_cpp() to generate .pb.h/cc" OFF)

# 3. 使用 protobuf 库和头文件
option(ENABLE_PROTOBUF_LIB_WAY "set ON to use existing protobuf library" OFF)

# 单元测试开关
option(UNIT_TEST "set ON to enable unit test" OFF)

###############################################

set(LIB_NAME "protobuf_static")
aux_source_directory(src PB_SRCS)

if (ENABLE_PROTOC_WAY)
    # 引入 Protobuf 库
    find_package(Protobuf 3 REQUIRED)

    # 生成 Protobuf 源文件和头文件
    file(GLOB_RECURSE PROTOS ${CMAKE_CURRENT_LIST_DIR}/*.proto)
    foreach (proto ${PROTOS})
        execute_process(COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} --proto_path=${CMAKE_CURRENT_LIST_DIR} --cpp_out=${CMAKE_CURRENT_LIST_DIR} ${proto})
    endforeach ()

    file(GLOB PROTO_SRCS proto/*.cc)
    add_library(${LIB_NAME} STATIC ${PB_SRCS} ${PROTO_SRCS})
    target_include_directories(
            ${LIB_NAME} PUBLIC
            include/
            proto/
            ${PROTOBUF_INCLUDE_DIRS}
    )
    target_link_libraries(${LIB_NAME} ${PROTOBUF_LIBRARIES})
endif ()

if (ENABLE_CMAKE_WAY)
    # 引入 Protobuf 库
    find_package(Protobuf 3 REQUIRED)

    # 生成 Protobuf 源文件和头文件
    file(GLOB_RECURSE PROTOS ${CMAKE_CURRENT_LIST_DIR}/proto/*.proto)
    foreach (proto ${PROTOS})
        protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${proto})
    endforeach ()

    add_library(${LIB_NAME} STATIC ${PB_SRCS} ${PROTO_SRCS})
    target_include_directories(
            ${LIB_NAME} PUBLIC
            include/
            ${Protobuf_INCLUDE_DIRS}
            ${CMAKE_CURRENT_BINARY_DIR}
    )
    target_link_libraries(${LIB_NAME} ${Protobuf_LIBRARIES} pthread)
endif ()

if (ENABLE_PROTOBUF_LIB_WAY)
    aux_source_directory(proto/ PROTO_SRCS)
    add_library(${LIB_NAME} STATIC ${PB_SRCS} ${PROTO_SRCS})
    target_include_directories(
            ${LIB_NAME} PUBLIC
            include/
            proto/
            protobuf/linux/include/
    )
#    find_library(PROTOBUF_LIB libprotobuf.a protobuf/linux/lib/)
    find_library(PROTOBUF_LIB libprotobuf.so protobuf/linux/lib/)
    target_link_libraries(${LIB_NAME}
            ${PROTOBUF_LIB}
            pthread
    )
endif ()

if (UNIT_TEST)
    add_executable(runPb test/test.cpp)
    target_link_libraries(runPb ${LIB_NAME})
endif ()