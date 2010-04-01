CC=gcc
CCOPTS=-Wall -std=c99
CCFLAGS=-std=c99
DEBUG=-g -D=DEBUG
DEST=batbeep

HEADERS =   src/daemon.h    \
            src/beep.h      \
            src/hashmap.h   \
            src/property.h

SOURCE =    src/batbeep.c   \
            src/daemon.c    \
            src/beep.c      \
            src/hashmap.c   \
            src/property.c

all: $(DEST)

debug: CCFLAGS += $(DEBUG)
debug: $(DEST)

$(DEST): $(SOURCE) $(HEADERS) Makefile
	$(CC) $(CCOPTS) $(SOURCE) -o $(DEST) $(CCFLAGS)

clean:
	rm $(DEST)
