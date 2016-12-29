.PHONY: all
all: termdict

.PHONY: install
install:
	cp obj/termdict /usr/bin

.PHONY: clean
clean:
	rm -rf obj/*

termdict: obj/util.o src/termdict.c
	gcc -Wall -pedantic -std=c99 -g -c src/termdict.c -o obj/termdict.o
	gcc -Wall -pedantic -std=c99 -g -o obj/termdict obj/termdict.o obj/util.o -lX11

obj/util.o: src/util.c
	gcc -Wall -pedantic -std=c99 -g -c src/util.c -o obj/util.o -lX11
