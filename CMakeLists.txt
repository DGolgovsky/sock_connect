cmake_minimum_required(VERSION 3.8)
project(sock_connect)

if (DEBUG)
    set(TARGET sock_connect_devel)
else()
    set(TARGET sock_connect)
endif()

if(DEFINED ENV{BUILD_NUMBER})
    project(${TARGET} VERSION 0.0.$ENV{BUILD_NUMBER})
else()
    project(${TARGET} VERSION 0.0.1)
endif()

set(CMAKE_CXX_STANDARD 14)

file(GLOB TARGET_SRC
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/src/*.h")

add_library(${TARGET} SHARED ${TARGET_SRC})

target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Wpedantic -O2 -fPIC)

install(TARGETS ${TARGET} LIBRARY DESTINATION lib)

if (DEBUG)
    target_compile_options(${TARGET} PRIVATE -DNDEBUG)
    install(DIRECTORY src/ DESTINATION include/devel
            FILES_MATCHING PATTERN "*.h")
else()
    install(DIRECTORY src/ DESTINATION include
            FILES_MATCHING PATTERN "*.h")
endif()

set(CPACK_GENERATOR DEB)

set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_VERSION_BUILD ${BUILD_NUMBER})
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE amd64)
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Prototype shared library of socket connector")
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Dmitry Golgovsky")

include(CPack)
