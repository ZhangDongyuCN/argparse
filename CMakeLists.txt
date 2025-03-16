# Three build variables:
# - ARCH: The platform of the bin file will run. Select from [x86, arm], default value is `x86`.
# - STATIC: Build statically linked binary and libraries. Select from [ON, OFF], default value is `OFF` (dynamically linked).
# - CMAKE_BUILD_TYPE: Release or Debug version. Select from [Release, Debug], default value is `Release`.
#
# Using Example：
#
# Build x86 release version:
# ```
# rm -rf build; mkdir build; cd build; cmake ..; make
# ```
#
# Build x86 debug version:
# ```
# rm -rf build; mkdir build; cd build; cmake -DCMAKE_BUILD_TYPE=Debug ..; make
# ```
#
# Build x86 release statically linked version:
# ```
# rm -rf build; mkdir build; cd build; cmake -DSTATIC=ON ..; make
# ```
#
# Build arm release version:
# ```
# rm -rf build; mkdir build; cd build; cmake -DARCH=arm ..; make
# ```

cmake_minimum_required(VERSION 3.0)

set(ARCH x86)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
option(STATIC "Build static binary and libraries." OFF)

if(NOT (UNIX OR LINUX))
    message(FATAL_ERROR "This CMakeLists.txt only supports Linux platforms, please write it yourself for other platforms.")
endif()

if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR)
    message(FATAL_ERROR "The binary directory of CMake cannot be the same as source directory!")
endif()

if(ARCH STREQUAL "x86")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/my_release/x86")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/my_release/x86/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/my_release/x86/lib")
    message("Build x86 version.")
elseif(ARCH STREQUAL "arm")
    set(CMAKE_C_COMPILER /opt/aarch64_eabi_gcc9.2.0_glibc2.31.0_fp/bin/aarch64-unknown-linux-gnueabi-gcc)
    set(CMAKE_CXX_COMPILER /opt/aarch64_eabi_gcc9.2.0_glibc2.31.0_fp/bin/aarch64-unknown-linux-gnueabi-g++)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/my_release/arm")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/my_release/arm/lib")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/my_release/arm/lib")
    message("Build arm version.")
else()
    message("Variable ARCH must select from [x86, arm].")
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set(CMAKE_BUILD_TYPE Release)
    message("Build Release version.")
elseif(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_BUILD_TYPE Debug)
    message("Build Debug version.")
else()
    message("Variable CMAKE_BUILD_TYPE must select from [Release, Debug].")
endif()

if(STATIC)
    add_link_options(-static)
    message("Build statically linked version.")
else()
    message("Build dynamically linked version.")
endif()

add_compile_options(-std=c++17 -Wall -Wextra -Werror)

project(argparse LANGUAGES C CXX)

# 编译 argparse
aux_source_directory(${CMAKE_SOURCE_DIR}/src argparse_src)
add_library(argparse_obj OBJECT ${argparse_src})
target_include_directories(argparse_obj PUBLIC ${CMAKE_SOURCE_DIR}/include)

# 编译测试程序
add_executable(test_argparse ${CMAKE_SOURCE_DIR}/test/test_argparse.cpp)
target_link_libraries(test_argparse PRIVATE argparse_obj)
