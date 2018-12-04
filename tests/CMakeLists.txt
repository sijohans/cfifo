project(cfifo)

macro(do_test arg)
	add_executable(${arg} ${arg})
	add_dependencies(${arg} ${arg} cfifo)
	add_sanitizers(${arg})
	add_test(${arg} ${arg})
	target_link_libraries(${arg} cfifo)
endmacro(do_test)

include_directories (../src)

# Test runners
do_test(run_test.c)
do_test(run_test.cpp)