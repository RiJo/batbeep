CC       =  gcc
CCFLAGS  =  -O2 -march=core2 -Wall -std=c99
DEBUG    =  -g -D=_DEBUG_
INCLUDES =  -I/usr/include
LDPATH   =  -L/usr/lib64
LIBS     =  
NAME     =  batbeep

HEADERS =   src/batbeep.h   \
            src/daemon.h    \
            src/beep.h      \
            src/hashmap.h   \
            src/property.h  \

OBJS     =  batbeep.o       \
            daemon.o        \
            beep.o          \
            hashmap.o       \
            property.o      \

all: $(NAME)

debug: CCFLAGS += $(DEBUG)
debug: $(NAME)

$(NAME): $(OBJS) Makefile
	$(CC) $(OBJS) $(LDPATH) $(LIBS) -o $@

$(OBJS): %.o: ./src/%.c $(HEADERS)
	$(CC) $(CFLAGS) $(CCFLAGS) $(DEFINES) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(NAME) $(OBJS)