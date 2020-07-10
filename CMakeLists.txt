cmake_minimum_required(VERSION 3.5 FATAL_ERROR)

# Set CMake library search policy
if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
	cmake_policy(SET CMP0005 NEW)
endif()
cmake_policy(SET CMP0016 NEW)
if(POLICY CMP0042)
	cmake_policy(SET CMP0042 NEW)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
include(TargetArch)

project(sock_connect LANGUAGES CXX)
find_package(Threads)

# Options
option(WITH_TESTS "Build unit tests" OFF)
option(WITH_TESTS_COVERAGE "Analyze test coverage using gcov (only for gcc)" OFF)
option(WITH_ASAN "Build ASan+UBSan instrumented code" OFF)
option(WITH_TSAN "Build TSan instrumented code" OFF)
option(ENABLE_UNIT_TEST "Enable unit test" ON)
option(ENABLE_STRESS_TEST "Enable stress test" ON)

set(CMAKE_TARGET_ARCHITECTURE "" CACHE STRING "Target build architecture")
set(SC_SHARED_LIBRARY ${PROJECT_NAME})
set(SC_STATIC_LIBRARY ${PROJECT_NAME}-static)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

if(BUILD_VERSION)
	set(PROJECT_VERSION 0.1.${BUILD_VERSION})
else()
	set(PROJECT_VERSION 0.1.5)
endif()

if(NOT CMAKE_TARGET_ARCHITECTURE)
	target_architecture(CMAKE_TARGET_ARCHITECTURE)
endif()

if(BIN_DIR)
	set(EXECUTABLE_OUTPUT_PATH ${BIN_DIR})
	set(LIBRARY_OUTPUT_PATH ${BIN_DIR})
else()
	set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR})
	set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR})
endif()
message(STATUS "Binary output path: ${EXECUTABLE_OUTPUT_PATH}")

if(NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE Debug CACHE STRING "Default build type to Debug" FORCE)
endif()

if(APPLE)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_DARWIN_C_SOURCE")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_DARWIN_C_SOURCE")
endif()

if(CMAKE_SYSTEM_NAME STREQUAL "AIX")
	set(CMAKE_CXX_ARCHIVE_CREATE "<CMAKE_AR> -q -c ${CMAKE_STATIC_LINKER_FLAGS} -o <TARGET> <OBJECTS>")
endif()

if(WITH_ASAN)
	if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1 -fPIC -fsanitize=address,undefined -DSC_ADDRESS_SANITIZER_ENABLED -fno-omit-frame-pointer -fno-optimize-sibling-calls")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O1 -fPIC -fsanitize=address,undefined -DSC_ASAN_ENABLED -fno-omit-frame-pointer -fno-optimize-sibling-calls")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address,undefined -pie")
	else()
		message(WARNING "Compiler does not support AddressSanitizer")
	endif()
endif(WITH_ASAN)

if(WITH_TSAN)
	if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O1 -fPIC -fsanitize=thread -DSC_THREAD_SANITIZER_ENABLED -fno-omit-frame-pointer")
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O1 -fPIC -fsanitize=thread -DSC_THREAD_SANITIZER_ENABLED -fno-omit-frame-pointer")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=thread -pie")
	else()
		message(WARNING "Compiler does not support ThreadSanitizer")
	endif()
endif(WITH_TSAN)

if(WITH_TESTS_COVERAGE)
	if(CMAKE_COMPILER_IS_GNUCXX)
		set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fprofile-arcs -ftest-coverage")
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs -ftest-coverage")
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fprofile-arcs -ftest-coverage")
		message(STATUS "Test coverage analysis: activated")
	else()
		message(WARNING "Compiler is not GNU gcc! Test coverage couldn't be analyzed")
	endif()
endif(WITH_TESTS_COVERAGE)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC")

message(STATUS "Build type -- ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler version: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME} version: ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Target architecture: ${CMAKE_TARGET_ARCHITECTURE}")
message(STATUS "Linker flags: ${CMAKE_EXE_LINKER_FLAGS}")

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
	string(REGEX MATCHALL "-std=[^ ]+" cxx_std_found ${CMAKE_CXX_FLAGS} " dummy@rg")
	if(cxx_std_found)
		message(STATUS "C++std: ${cxx_std_found}")
	else()
		set(CMAKE_CXX_STANDARD 11)
		list(APPEND PUBLIC_CXX_FLAGS "-std=c++${CMAKE_CXX_STANDARD}")
		message(STATUS "C++std: -std=c++${CMAKE_CXX_STANDARD} (default)")
	endif()

	list(APPEND PUBLIC_CXX_FLAGS "-Wall" "-Wextra" "-Werror" "-pedantic")
	list(APPEND PUBLIC_CXX_FLAGS "-march=native" "-mtune=generic" "-pipe" "-fPIC" "-fstack-protector-strong" "-fno-plt")
	if(CMAKE_TARGET_ARCHITECTURE STREQUAL "x86_64")
		list(APPEND PUBLIC_CXX_FLAGS "-mcx16")
		set(LIB_SUFFIX "64")

		# GCC-7: 128-bit atomics support is implemented via libatomic on amd64
		#        see https://gcc.gnu.org/ml/gcc/2017-01/msg00167.html
		# Maybe, it will be changed in future
		if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "7.0.0" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0.0")
			set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -latomic")
		endif()
	endif()

	if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0.0")
		#   gcc 4.8 - 6: disable noise -Wunused-local-typedefs
		list(APPEND PRIVATE_CXX_FLAGS "-Wno-unused-local-typedefs")
	endif()

	if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
		list(APPEND PRIVATE_CXX_FLAGS "-Weverything" "-stdlib=libc++" "-pthread")
		list(APPEND PRIVATE_CXX_FLAGS "-Wno-null-dereference")
		list(APPEND PRIVATE_CXX_FLAGS "-Wno-sign-conversion")
		list(APPEND PRIVATE_CXX_FLAGS "-Wno-unused-local-typedef")
		list(APPEND PRIVATE_CXX_FLAGS "-Wthread-safety")
	else()
		list(APPEND PRIVATE_CXX_FLAGS
		     "-Wctor-dtor-privacy"
		     "-Wenum-compare"
		     "-Wfloat-equal"
		     "-Wnon-virtual-dtor"
		     "-Woverloaded-virtual"
		     "-Wredundant-decls"
		     "-Wconversion"
		     "-Wno-unused-parameter"
		     "-Wold-style-cast"
		     "-Wpointer-arith"
		     "-Wshadow"
		     "-Wwrite-strings")
	endif()
endif()

if(CMAKE_BUILD_TYPE MATCHES "Debug")
	message(STATUS "Cmake flags:${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
else()
	add_compile_options(-O3)
	message(STATUS "Cmake flags:${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_RELEASE}")
endif()
message(STATUS "Compiler public flags: ${PUBLIC_CXX_FLAGS}")
message(STATUS "Compiler private flags: ${PRIVATE_CXX_FLAGS}")

file(GLOB TARGET_SRC "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_library(${SC_SHARED_LIBRARY} SHARED ${TARGET_SRC})
add_library(${SC_STATIC_LIBRARY} STATIC ${TARGET_SRC})

set_target_properties(${SC_SHARED_LIBRARY} PROPERTIES VERSION ${PROJECT_VERSION} DEBUG_POSTFIX "_d")
set_target_properties(${SC_STATIC_LIBRARY} PROPERTIES DEBUG_POSTFIX "_d")
set_target_properties(${SC_STATIC_LIBRARY} PROPERTIES OUTPUT_NAME ${PROJECT_NAME})

target_compile_options(${SC_SHARED_LIBRARY} PUBLIC  "${PUBLIC_CXX_FLAGS}")
target_compile_options(${SC_SHARED_LIBRARY} PRIVATE "${PRIVATE_CXX_FLAGS}")
target_compile_options(${SC_STATIC_LIBRARY} PUBLIC  "${PUBLIC_CXX_FLAGS}")
target_compile_options(${SC_STATIC_LIBRARY} PRIVATE "${PRIVATE_CXX_FLAGS}")

target_link_libraries(${SC_SHARED_LIBRARY} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${SC_STATIC_LIBRARY} PRIVATE ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(${SC_SHARED_LIBRARY} INTERFACE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
                           $<INSTALL_INTERFACE:include>)
target_include_directories(${SC_STATIC_LIBRARY} INTERFACE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>"
                           $<INSTALL_INTERFACE:include>)
target_include_directories(${SC_SHARED_LIBRARY} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_include_directories(${SC_STATIC_LIBRARY} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
	set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")
	set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CMAKE_COMMAND} -E time")

	add_executable(tcp_socket_test tests/tcp_socket_test.cpp)
	target_include_directories(tcp_socket_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
	target_link_libraries(tcp_socket_test ${SC_SHARED_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
	set_target_properties(tcp_socket_test PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)

	add_executable(udp_socket_test tests/udp_socket_test.cpp)
	target_include_directories(udp_socket_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
	target_link_libraries(udp_socket_test ${SC_SHARED_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
	set_target_properties(udp_socket_test PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)

	add_executable(unix_socket_test tests/unix_socket_test.cpp)
	target_include_directories(unix_socket_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
	target_link_libraries(unix_socket_test ${SC_SHARED_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
	set_target_properties(unix_socket_test PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)
endif()

install(TARGETS ${SC_SHARED_LIBRARY} EXPORT LibSCConfig LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT ${LIBRARIES_COMPONENT} NAMELINK_SKIP)
install(TARGETS ${SC_SHARED_LIBRARY} EXPORT LibSCConfig LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT ${HEADERS_COMPONENT} NAMELINK_ONLY)
install(TARGETS ${SC_STATIC_LIBRARY} EXPORT LibSCConfig DESTINATION lib${LIB_SUFFIX} COMPONENT ${LIBRARIES_COMPONENT})
install(EXPORT LibSCConfig FILE LibSCConfig.cmake DESTINATION lib/cmake/LibSC)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ RUNTIME DESTINATION include COMPONENT ${HEADERS_COMPONENT} FILES_MATCHING PATTERN "*.h")

set(CPACK_GENERATOR TGZ;STGZ;ZIP;DEB;RPM)

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_BUILD ${BUILD_VERSION})
set(CPACK_PACKAGE_RELEASE 1)
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_RESOURCE_FILE_LICENSE ${PROJECT_SOURCE_DIR}/LICENSE)
set(CPACK_PACKAGE_CHECKSUM SHA256)
set(CPACK_PACKAGE_FILE_NAME "lib${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION}-${CPACK_PACKAGE_RELEASE}-${CMAKE_HOST_SYSTEM_PROCESSOR}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Prototype shared library of socket connector")
set(CPACK_PACKAGE_CONTACT "Dmitry Golgovsky <d.westcoast@aol.com>")

# TGZ specific
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

# RPM specific
set(CPACK_RPM_COMPONENT_INSTALL ON)
set(CPACK_RPM_PACKAGE_RELEASE ${CPACK_PACKAGE_RELEASE})
set(CPACK_RPM_PACKAGE_URL https://github.com/dgolgovsky/sock_connect)
set(CPACK_RPM_PACKAGE_LICENSE MIT)
set(CPACK_RPM_PACKAGE_GROUP "System Environment/Base")
set(CPACK_RPM_PACKAGE_REQUIRES "glibc, gcc")
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION ${CPACK_PACKAGING_INSTALL_PREFIX})
set(CPACK_RPM_EXCLUDE_FROM_AUTO_FILELIST_ADDITION /usr/local)

# DEB specific
set(CPACK_DEB_COMPONENT_INSTALL ON)
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE ${CMAKE_TARGET_ARCHITECTURE})
set(CPACK_DEB_PACKAGE_LICENSE MIT)
set(CPACK_DEBIAN_PACKAGE_DEPENDS "glibc, gcc")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://github.com/dgolgovsky/sock_connect")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Dmitry Golgovsky")

# Components grouping for Mac OS X and Windows installers
set(CPACK_COMPONENT_${LIBRARIES_COMPONENT}_GROUP "Runtime")
set(CPACK_COMPONENT_${HEADERS_COMPONENT}_GROUP "Development")
set(CPACK_COMPONENT_${LIBRARIES_COMPONENT}_DISPLAY_NAME "Libraries")
set(CPACK_COMPONENT_${HEADERS_COMPONENT}_DISPLAY_NAME "C++ Headers")
set(CPACK_COMPONENT_${HEADERS_COMPONENT}_DEPENDS ${LIBRARIES_COMPONENT})

include(CPack)

# uninstall target
if(NOT TARGET uninstall)
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	               IMMEDIATE @ONLY)

	add_custom_target(uninstall
	                  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()

if(TESTS)
	message(STATUS "Build tests: activated")
	enable_testing()
	file(GLOB TESTS_SRC
	     "${CMAKE_CURRENT_SOURCE_DIR}/tests/catch_test/*.cpp"
	     "${CMAKE_CURRENT_SOURCE_DIR}/tests/catch_test/*.h")
	add_executable(catch_test ${TESTS_SRC})
	add_test(catch_test COMMAND)
endif()
