project(cfifo)

macro(do_test arg)
	add_executable(${arg} ${arg})
	add_dependencies(${arg} ${arg} cfifo)
	add_sanitizers(${arg})
	add_test(${arg} ${arg})
	target_link_libraries(${arg} cfifo_generic)
endmacro(do_test)

include_directories (../src)

# Test runners
set_source_files_properties(run_test.c
	PROPERTIES
	COMPILE_FLAGS
	-std=c99
	-Wextra -Wpedantic -Wall -Werror)
do_test(run_test.c)
do_test(run_test.cpp)
do_test(c89_test.c)
