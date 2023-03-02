set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wextra")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wpedantic")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weffc++")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-long-long")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wconversion")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wctor-dtor-privacy")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Woverloaded-virtual")

list(APPEND PRIVATE_CXX_FLAGS
     "-pipe"
     "-fstack-protector-strong"
     "-fno-plt"
     "-fsized-deallocation"
     "-fPIC"
     "-fexceptions"
     "-fstack-clash-protection"
     "-fstack-protector-strong"
     "-fcf-protection"
     "-fomit-frame-pointer"
     "-ffunction-sections"
     "-fdata-sections")

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    list(APPEND PRIVATE_CXX_FLAGS
         "-stdlib=libstdc++"
         "-pthread"
         "-fexperimental-new-pass-manager"
         "-mllvm"
         "-inline-threshold=1000"
         )
    list(APPEND PUBLIC_CXX_FLAGS
         "-Weverything"
         "-Wno-c++98-compat"
         "-Wno-c++98-compat-pedantic"
         "-Wno-null-dereference"
         "-Wno-sign-conversion"
         "-Wno-unused-local-typedef"
         "-Wthread-safety")
else()
    list(APPEND PUBLIC_CXX_FLAGS
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

if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CMAKE_COMMAND} -E time")
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK "${CMAKE_COMMAND} -E time")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0")
endif()

# GCC-7: 128-bit atomics support is implemented via libatomic on amd64
#        see https://gcc.gnu.org/ml/gcc/2017-01/msg00167.html
# Maybe, it will be changed in future
if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "7.0.0" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0.0")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -latomic")
endif()

if(CMAKE_COMPILER_IS_GNUCXX AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "8.0.0")
    #   gcc 4.8 - 6: disable noise -Wunused-local-typedefs
    list(APPEND PRIVATE_CXX_FLAGS "-Wno-unused-local-typedefs")
endif()

if(CMAKE_TARGET_ARCHITECTURE STREQUAL "x86_64")
    list(APPEND PRIVATE_CXX_FLAGS
         "-mmmx"
         "-msse"
         "-fopenmp-simd"
         "-mtune=native"
         "-march=native"
         "-mavx")
    set(LIB_SUFFIX "64")
endif()

if(WITH_ASAN)
    if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        list(APPEND PRIVATE_CXX_FLAGS
             "-fsanitize=address,undefined,pointer-compare"
             "-fno-sanitize-recover=all"
             "-fsanitize=float-divide-by-zero"
             "-fsanitize=float-cast-overflow"
             "-fno-sanitize=null"
             "-fno-sanitize=alignment"
             "-fno-omit-frame-pointer"
             "-fno-optimize-sibling-calls")
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

string(REGEX MATCHALL "-std=[^ ]+" cxx_std_found ${CMAKE_CXX_FLAGS} " dummy@rg")
if(cxx_std_found)
    message(STATUS "C++std: ${cxx_std_found}")
else()
    #list(APPEND PUBLIC_CXX_FLAGS "-std=c++${CMAKE_CXX_STANDARD}")
    message(STATUS "C++std: -std=c++${CMAKE_CXX_STANDARD} (default)")
endif()
