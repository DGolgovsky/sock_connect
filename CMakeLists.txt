cmake_minimum_required(VERSION 3.5 FATAL_ERROR)
project(SOCK_CONNECT LANGUAGES CXX)

set(TARGET sock_connect)

find_package(Threads)

if (DEFINED ENV{BUILD_NUMBER})
	project(${TARGET} VERSION 0.$ENV{BUILD_NUMBER})
else ()
	project(${TARGET} VERSION 0.5)
endif ()

add_compile_options(-O2 -fPIC)

file(GLOB TARGET_SRC
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
		"${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_library(${TARGET} SHARED ${TARGET_SRC})

set_target_properties(${TARGET} PROPERTIES
		CXX_STANDARD 11
		CXX_STANDARD_REQUIRED ON)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
	set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS "-Weverything -stdlib=libc++")
else ()
	set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wpedantic")
endif ()

target_link_libraries(${TARGET} ${CMAKE_THREAD_LIBS_INIT})

install(TARGETS ${TARGET} LIBRARY DESTINATION lib)

if (DEBUG)
	target_compile_options(${TARGET} PRIVATE -DNDEBUG -g)
endif ()

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
