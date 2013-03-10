CC=gcc

WARNING_FLAGS=-Wall -Wextra -Werror-implicit-function-declaration -Wshadow -Wstrict-prototypes -pedantic-errors
CFLAGS=-g -O2 -std=c99 $(WARNING_FLAGS)
LDFLAGS=-g -lGL -lglut -lGLU

.c.o:
	$(CC) -c $(CFLAGS) $<

all: main
main: geometry.o main.o polys.o ppmio.o
	$(CC) $(LDFLAGS) -o main geometry.o main.o polys.o ppmio.o

clean:
	rm -f *.o
	rm -f main

geometry.o : geometry.h polys.h geometry.c
ppmio.o    : ppmio.c
geometry.o : polys.h geometry.h
ppmio.o    : ppmio.h
main.o     : geometry.h polys.h ppmio.h main.c
polys.o    : polys.h polys.c
