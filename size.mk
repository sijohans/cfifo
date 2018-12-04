.PHONY: size clean

CFLAGS := -O3 -Isrc

build:
	mkdir -p build

cfifo_specific.o: build src/cfifo_specific.c
	$(CC) $(CFLAGS) -c src/cfifo_specific.c -o build/cfifo_specific.o

cfifo_generic.o: build src/cfifo_generic.c
	$(CC) $(CFLAGS) -c src/cfifo_generic.c -o build/cfifo_generic.o

size: cfifo_specific.o cfifo_generic.o
	size build/cfifo_specific.o build/cfifo_generic.o
