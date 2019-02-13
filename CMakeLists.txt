cmake_minimum_required(VERSION 3.5)

project(SOCK_CONNECT LANGUAGES CXX)

set(TARGET sock_connect)

if(DEFINED ENV{BUILD_NUMBER})
    project(${TARGET} VERSION 0.$ENV{BUILD_NUMBER})
else()
    project(${TARGET} VERSION 0.1)
endif()

file(GLOB TARGET_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_library(${TARGET} SHARED ${TARGET_SRC})

set_target_properties(${TARGET} PROPERTIES
        CXX_STANDARD 14
        CXX_STANDARD_REQUIRED ON)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS "-Weverything -stdlib=libc++ -fPIC")
else()
    set_target_properties(${TARGET} PROPERTIES COMPILE_FLAGS "-Wall -Wextra -Wpedantic -O2 -fPIC")
endif()

install(TARGETS ${TARGET} LIBRARY DESTINATION lib)

if (DEBUG)
    target_compile_options(${TARGET} PRIVATE -DNDEBUG)
endif()

install(DIRECTORY src/ DESTINATION include/
        FILES_MATCHING PATTERN "*.h")

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_BUILD ${BUILD_NUMBER})
#set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Prototype shared library of socket connector")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Dmitry Golgovsky")
set(CPACK_PACKAGE_CONTACT d.westcoast@aol.com)

include(CPack)
