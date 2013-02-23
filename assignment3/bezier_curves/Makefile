CC=gcc
CXX=g++

#BULLET=./bullet32
BULLET=./bullet64
#BULLET=./bullet64MAC

WARNING_FLAGS=-Wall -Wextra -Werror-implicit-function-declaration -Wshadow -Wstrict-prototypes -pedantic
CFLAGS=-g -O2 -std=c99 $(WARNING_FLAGS) -fno-strict-aliasing
CXXFLAGS=-g -O2 -W -Wall -I${BULLET}/include
LDFLAGS=-g -lGL -lglut -lGLU -L${BULLET}/lib
LIBS=-lBulletDynamics -lBulletCollision -lLinearMath

.c.o:
	$(CC) -c $(CFLAGS) $<
.cpp.o:
	$(CXX) -c $(CXXFLAGS) $<

all: singlecurve multicurve
singlecurve: singlecurve.o bezier.o
	$(CC) $(LDFLAGS) -o singlecurve singlecurve.o bezier.o
multicurve: multicurve.o bezier.o v3math.o plymodel.o rply.o physics.o
	$(CXX) $(LDFLAGS) -o multicurve multicurve.o bezier.o v3math.o plymodel.o rply.o physics.o $(LIBS)

clean:
	rm -f *.o
	rm -f singlecurve
	rm -f multicurve

bezier.o        : bezier.h bezier.c
singlecurve.o   : bezier.h singlecurve.c
multicurve.o    : bezier.h multicurve.c plymodel.h physics.h
v3math.o        : v3math.h v3math.c
plymodel.o      : plymodel.c plymodel.h
rply.o          : rply.c rply.h
physics.o       : physics.h
