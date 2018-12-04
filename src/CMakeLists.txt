project(cfifo)

add_library(cfifo_generic cfifo_generic.c)
add_sanitizers(cfifo_generic)
add_library(cfifo_specific cfifo_specific.c)
add_sanitizers(cfifo_specific)
