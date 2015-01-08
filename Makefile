.PHONY: clean
all: bin/liby4m.so

bin/liby4m.so: bin/liby4m.o bin/parse.o
	gcc -shared -o bin/liby4m.so bin/liby4m.o bin/parse.o

bin/parse.o: src/parse.c include/parse.h
	gcc -g -fpic -Wall -Werror -c -o bin/parse.o src/parse.c -Iinclude

bin/liby4m.o: src/liby4m.c include/liby4m.h
	gcc -g -fpic -Wall -Werror -c -o bin/liby4m.o src/liby4m.c -Iinclude

clean:
	rm bin/*