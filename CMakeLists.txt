cmake_minimum_required(VERSION 2.8)

project(cfifo)

enable_testing()
enable_language(C)

set(CMAKE_C_FLAGS_DEBUG " -O0 -g -ggdb -Wall -Werror -Wpedantic -coverage ${CMAKE_CXX_FLAGS_DEBUG}")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
find_package(Sanitizers)

add_subdirectory(src)
add_subdirectory(tests)
