cmake_minimum_required(VERSION 2.8)

project(cfifo)

enable_testing()
enable_language(C)

set(CMAKE_C_FLAGS_DEBUG " -O0 -g -ggdb -Wall -Werror -Wpedantic -fprofile-arcs -ftest-coverage ${CMAKE_CXX_FLAGS_DEBUG}")

add_subdirectory(src)
add_subdirectory(tests)