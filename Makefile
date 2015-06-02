.PHONY: clean
all: bin/liby4m.so

CFLAGS=-g -fpic -Wall -Werror -Iinclude

bin/liby4m.so: bin/liby4m.o bin/parse.o
	gcc -shared -o bin/liby4m.so bin/liby4m.o bin/parse.o

bin/parse.o: src/parse.c include/parse.h
	gcc $(CFLAGS) -c -o bin/parse.o src/parse.c

bin/liby4m.o: src/liby4m.c include/liby4m.h
	gcc $(CFLAGS) -c -o bin/liby4m.o src/liby4m.c

clean:
	rm bin/*
