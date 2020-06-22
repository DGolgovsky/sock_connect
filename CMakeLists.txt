cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(SOCK_CONNECT LANGUAGES CXX)

set(TARGET sock_connect)

find_package(Threads)

if(DEFINED ENV{BUILD_NUMBER})
	project(${TARGET} VERSION 0.$ENV{BUILD_NUMBER})
else()
	project(${TARGET} VERSION 0.5)
endif()

add_compile_options(-Wall -Wextra -Werror -Wpedantic)

file(GLOB TARGET_SRC
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_library(${TARGET} SHARED ${TARGET_SRC})

set_target_properties(${TARGET} PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)
target_compile_options(${TARGET} PRIVATE -march=native -mtune=generic -pipe -fPIC -fstack-protector-strong -fno-plt)
if(CMAKE_CXX_COMPILER_ID MATCHES "clang")
	set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS "-Weverything -stdlib=libc++ -pthread")
else()
	set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS
	                      "-Wctor-dtor-privacy -Wenum-compare -Wfloat-equal -Wnon-virtual-dtor -Woverloaded-virtual -Wredundant-decls")
endif()

target_link_libraries(${TARGET} ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS ${TARGET} LIBRARY DESTINATION lib)

if(NOT DEBUG)
	target_compile_options(${TARGET} PRIVATE -DNDEBUG)
	add_compile_options(-O3)
else()
	message(*\ Building\ with\ DEBUG\ Insertions)
	add_compile_options(-O2)
	target_compile_options(${TARGET} PRIVATE -g -D_GLIBCXX_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC)
endif()

add_executable(tcp_socket_test tests/tcp_socket_test.cpp)
target_include_directories(tcp_socket_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(tcp_socket_test ${TARGET} ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(tcp_socket_test PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)

add_executable(udp_socket_test tests/udp_socket_test.cpp)
target_include_directories(udp_socket_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_link_libraries(udp_socket_test ${TARGET} ${CMAKE_THREAD_LIBS_INIT})
set_target_properties(udp_socket_test PROPERTIES CXX_STANDARD 11 CXX_STANDARD_REQUIRED ON)

install(DIRECTORY src/ DESTINATION include
        FILES_MATCHING PATTERN "*.h")

set(CPACK_GENERATOR ${GEN_PKG})

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_BUILD ${BUILD_NUMBER})
#set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
SET(CPACK_PACKAGE_FILE_NAME "lib${CMAKE_PROJECT_NAME}-${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}-1-${CMAKE_HOST_SYSTEM_PROCESSOR}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Prototype shared library of socket connector")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Dmitry Golgovsky")
set(CPACK_PACKAGE_CONTACT d.westcoast@aol.com)

include(CPack)

# uninstall target
if(NOT TARGET uninstall)
	configure_file("${CMAKE_CURRENT_SOURCE_DIR}/cmake_uninstall.cmake.in"
	               "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
	               IMMEDIATE @ONLY)

	add_custom_target(uninstall
	                  COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()

if(TEST)
	enable_testing()
	file(GLOB TESTS_SRC
	     "${CMAKE_CURRENT_SOURCE_DIR}/tests/catch_test/*.cpp"
	     "${CMAKE_CURRENT_SOURCE_DIR}/tests/catch_test/*.h")
	add_executable(catch_test ${TESTS_SRC})
	add_test(catch_test COMMAND)
endif()
