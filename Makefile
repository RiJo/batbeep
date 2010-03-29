CC=gcc
CCOPTS=-Wall -g
CCFLAGS=-std=c99
DEBUG=-g -D=DEBUG
DEST=batbeep

HEADERS =   src/daemon.h    \
            src/beep.h      \
            src/property.h

SOURCE =    src/batbeep.c   \
            src/daemon.c    \
            src/beep.c      \
            src/property.c

LDPATH=-L.
LDLIBS=-lhashmap

all: $(DEST)

debug: CCFLAGS += $(DEBUG)
debug: $(DEST)

$(DEST): $(SOURCE) $(HEADERS) Makefile
	$(CC) $(CCOPTS) $(SOURCE) -o $(DEST) $(CCFLAGS) $(LDPATH) $(LDLIBS)

clean:
	rm $(DEST)