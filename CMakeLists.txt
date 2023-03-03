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

project(sock_connect LANGUAGES CXX)
if(BUILD_VERSION)
    set(PROJECT_VERSION 0.1.${BUILD_VERSION})
else()
    set(PROJECT_VERSION 0.1.999)
endif()

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
if(CMAKE_BUILD_TYPE MATCHES "Debug")
    include(StaticAnalyzers)
endif()

# Options
option(WITH_TESTS "Build unit tests" OFF)
option(WITH_TESTS_COVERAGE "Analyze test coverage using gcov (only for gcc)" OFF)
option(WITH_ASAN "Build ASan+UBSan instrumented code" OFF)
option(WITH_TSAN "Build TSan instrumented code" OFF)
option(ENABLE_UNIT_TEST "Enable unit test" ON)
option(ENABLE_STRESS_TEST "Enable stress test" ON)

set(CMAKE_TARGET_ARCHITECTURE "" CACHE STRING "Target build architecture")
set(SC_SHARED_LIBRARY ${PROJECT_NAME})
set(SC_STATIC_LIBRARY ${PROJECT_NAME}_static)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include(TargetArch)
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

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -D_DEBUG -D_GLIBCXX_DEBUG_PEDANTIC -g3 -ggdb -fstack-protector-all")

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Build version: ${PROJECT_VERSION}")
message(STATUS "Compiler version: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}")
message(STATUS "System: ${CMAKE_SYSTEM_NAME} version: ${CMAKE_SYSTEM_VERSION}")
message(STATUS "Target architecture: ${CMAKE_TARGET_ARCHITECTURE}")
message(STATUS "Linker flags: ${CMAKE_EXE_LINKER_FLAGS}")

include(project_cxx)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    message(STATUS "Cmake flags:${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}")
else()
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

target_compile_options(${SC_SHARED_LIBRARY} PUBLIC "${PUBLIC_CXX_FLAGS}")
target_compile_options(${SC_SHARED_LIBRARY} PRIVATE "${PRIVATE_CXX_FLAGS}")
target_compile_options(${SC_STATIC_LIBRARY} PUBLIC "${PUBLIC_CXX_FLAGS}")
target_compile_options(${SC_STATIC_LIBRARY} PRIVATE "${PRIVATE_CXX_FLAGS}")

find_package(Threads)
target_link_libraries(${SC_SHARED_LIBRARY} PRIVATE ${CMAKE_THREAD_LIBS_INIT})
target_link_libraries(${SC_STATIC_LIBRARY} PRIVATE ${CMAKE_THREAD_LIBS_INIT})

target_include_directories(${SC_SHARED_LIBRARY} INTERFACE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>" $<INSTALL_INTERFACE:interface>)
target_include_directories(${SC_STATIC_LIBRARY} INTERFACE "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}>" $<INSTALL_INTERFACE:interface>)
target_include_directories(${SC_SHARED_LIBRARY} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_include_directories(${SC_STATIC_LIBRARY} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)

include(GNUInstallDirs)
install(TARGETS ${SC_SHARED_LIBRARY} EXPORT LibSCConfig LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT ${LIBRARIES_COMPONENT} NAMELINK_SKIP)
install(TARGETS ${SC_SHARED_LIBRARY} EXPORT LibSCConfig LIBRARY DESTINATION lib${LIB_SUFFIX} COMPONENT ${HEADERS_COMPONENT} NAMELINK_ONLY)
install(TARGETS ${SC_STATIC_LIBRARY} EXPORT LibSCConfig DESTINATION lib${LIB_SUFFIX} COMPONENT ${LIBRARIES_COMPONENT})
install(EXPORT LibSCConfig FILE LibSCConfig.cmake DESTINATION lib/cmake/LibSC)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/interface/ RUNTIME DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} COMPONENT ${HEADERS_COMPONENT} FILES_MATCHING PATTERN "*.h")

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
    configure_file("${CMAKE_MODULE_PATH}/cmake_uninstall.cmake.in"
                   "${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake"
                   IMMEDIATE @ONLY)

    add_custom_target(uninstall
                      COMMAND ${CMAKE_COMMAND} -P ${CMAKE_CURRENT_BINARY_DIR}/cmake_uninstall.cmake)
endif()

if(TESTS)
    enable_testing()
    set(TEST_MODULES
        tcp_socket_test
        udp_socket_test
        sun_socket_test
        usb_socket_test)
    foreach(module ${TEST_MODULES})
        add_executable(${module} tests/${module}.cpp)
        target_link_libraries(${module} ${SC_SHARED_LIBRARY} ${CMAKE_THREAD_LIBS_INIT})
    endforeach()
    message(STATUS "Build tests: activated")
endif()
