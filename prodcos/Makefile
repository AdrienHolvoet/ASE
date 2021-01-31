LIBNAME=hardware

RM	= rm
CC	= gcc
LIBDIR  = lib
LDLIBS	= -L$(LIBDIR) -l$(LIBNAME)

all: prodcons

prodcons.o: prodcons.c

prodcons: prodcons.o

clean:
	$(RM) -f *.o tmr vdisk?.bin

.PHONY: clean
