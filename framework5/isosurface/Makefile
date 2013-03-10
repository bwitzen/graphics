CC=gcc

WARNING_FLAGS=-Wall -Wextra -Werror-implicit-function-declaration -Wshadow -Wstrict-prototypes -pedantic-errors
CFLAGS=-g -O2 -std=c99 $(WARNING_FLAGS)
LDFLAGS=-g -lGL -lglut -lGLU

.c.o:
	$(CC) -c $(CFLAGS) $<

all: main
main: main.o marching_tetrahedra.o volume.o
	$(CC) $(LDFLAGS) -o main main.o marching_tetrahedra.o volume.o

clean:
	rm -f *.o
	rm -f main

volume.o              : v3math.h volume.h
volume.o              : volume.h volume.c
v3math.o              : v3math.h
marching_tetrahedra.o : marching_tetrahedra.h marching_tetrahedra.c
marching_tetrahedra.o : v3math.h volume.h marching_tetrahedra.h
main.o                : marching_tetrahedra.h v3math.h volume.h main.c
