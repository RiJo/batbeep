CC=gcc
CCOPTS=-Wall -g
CCFLAGS=-std=c99
DEBUG=-g -D=DEBUG
SRC_DIR=./src
BIN_DIR=./bin
DEST=$(BIN_DIR)/batbeep
HEADERS=$(SRC_DIR)/daemon.h $(SRC_DIR)/property.h
SOURCE=$(SRC_DIR)/batbeep.c $(SRC_DIR)/daemon.c $(SRC_DIR)/property.c
LDPATH=-L.
LDLIBS=-lhashmap -lbeep

all: $(DEST)

$(DEST): $(SOURCE) $(HEADERS) Makefile
	$(CC) $(CCOPTS) $(SOURCE) -o $(DEST) $(CCFLAGS) $(LDPATH) $(LDLIBS)

debug: $(SOURCE) $(HEADERS) Makefile
	$(CC) $(CCOPTS) $(SOURCE) -o $(DEST) $(CCFLAGS) $(DEBUG) $(LDPATH) $(LDLIBS)

clean:
	rm $(DEST)
