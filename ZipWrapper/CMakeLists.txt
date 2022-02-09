cmake_minimum_required(VERSION 3.4.1)
set(PROJECT_NAME ZipWrapper)
project(${PROJECT_NAME})

if (NOT _INCLUDE_ZIP_WRAPPER_)
	message(STATUS "正在编译 ${PROJECT_NAME}")
	set(_INCLUDE_ZIP_WRAPPER_ YES CACHE STRING DOCSTRING FORCE)
	set(CMAKE_CXX_STANDARD 14)

	# 本工程的源文件
	file(GLOB_RECURSE ZIP_SRCS Source/*.c Source/*.cpp)

	# 编译成静态库
	set(ZIPWRAPPER_LIB_NAME ZipWrapper_static)
	add_library(${ZIPWRAPPER_LIB_NAME} STATIC ${ZIP_SRCS})

	# 本工程头文件引用路径
	target_include_directories(
			${ZIPWRAPPER_LIB_NAME} PUBLIC
			Export_Inc
			Source/Zip
	)

	# 测试
	function(testZipWrapper)
		add_compile_options(-g)
		add_executable(runZipWrapper test/test.cpp)
		target_link_libraries(
				runZipWrapper
				${ZIPWRAPPER_LIB_NAME}
		)
	endfunction()
	testZipWrapper()

endif()