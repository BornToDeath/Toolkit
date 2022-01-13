cmake_minimum_required(VERSION 3.4.1)
set(PROJECT_NAME ZipWrapper)
project(${PROJECT_NAME})

if (NOT _INCLUDE_ZIP_WRAPPER_)
	message(STATUS "正在编译 ${PROJECT_NAME}")
	set(_INCLUDE_ZIP_WRAPPER_ YES CACHE STRING DOCSTRING FORCE)
	set(CMAKE_CXX_STANDARD 14)

	# 本工程的源文件
	aux_source_directory(Source ZIPWRAPPER_SRCS)

	# 编译成静态库
	set(ZIPWRAPPER_LIB_NAME ZipWrapper_static)
	add_library(${ZIPWRAPPER_LIB_NAME} STATIC ${ZIPWRAPPER_SRCS})

	# 本工程头文件引用路径
	target_include_directories(
			${ZIPWRAPPER_LIB_NAME} PUBLIC
			Export_Inc
			../../ThirdPartyLayer/Zip/Source
	)

	# 本工程引用的外部项目
	add_subdirectory(../../ThirdPartyLayer/Zip Zip.out)

	# 链接的库文件
	target_link_libraries(
			${ZIPWRAPPER_LIB_NAME}
			Zip_static
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