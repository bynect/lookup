all:

lookup.o: lookup.c lookup.h
	${CC} lookup.c -o lookup.o -I. -c

example: lookup.o
	${CC} example.c lookup.o -o example

.PHONY: clean
clean:
	rm -rf lookup.o example
