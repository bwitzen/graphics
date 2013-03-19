PROGNAME = main
SRCFILES = main.cpp levels.cpp
TARNAME = box2d.tgz

CC = g++

# Ubuntu uses libbox2d instead of libBox2D.
LBOX2D = $(shell test -f /etc/debian_version && echo -lbox2d || echo -lBox2D)

WARNFLAGS = -Wall -Wshadow -pedantic-errors
CFLAGS = -g -O2 $(WARNFLAGS) -Iinclude/
LFLAGS = -lGL -lglut $(LBOX2D)

# Do some substitution to get a list of .o files from the given .cpp files.
OBJFILES = $(patsubst %.cpp,%.o,$(SRCFILES))

.PHONY: all run clean dist

all: $(PROGNAME)

$(PROGNAME): $(OBJFILES)
	$(CC) $(CFLAGS) -o $@ $^ $(LFLAGS)

%.o: %.cpp
	$(CC) -c $(CFLAGS) -o $@ $<

run: $(PROGNAME)
	LD_LIBRARY_PATH=/usr/local/lib ./$(PROGNAME)

dist:
	tar cvzf $(TARNAME) Makefile *.cpp *.h

clean:
	rm -fv $(PROGNAME) $(OBJFILES) $(TARNAME)
