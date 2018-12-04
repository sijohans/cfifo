cmake_minimum_required(VERSION 2.8)

project(cfifo)

enable_testing()
enable_language(C)

set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -std=c99 -g -ggdb -Werror -Wall -Wextra -Wpedantic -Wshadow -Wcast-qual ")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -std=c++11 -g -ggdb -Werror -Wall -Wextra -Wpedantic -Wshadow ")

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})
find_package(Sanitizers)

add_subdirectory(src)
add_subdirectory(tests)
