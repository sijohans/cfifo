project(cfifo)

macro(do_test arg)
	add_executable(${arg} ${arg}.c)
	add_dependencies(${arg} ${arg} cfifo)
	add_test(${arg} ${arg})
	target_link_libraries(${arg} cfifo)
endmacro(do_test)

include_directories (../src)

# Test runners
do_test(run_test)