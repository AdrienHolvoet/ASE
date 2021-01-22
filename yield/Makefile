LIBNAME=hardware

RM	= rm
CC	= gcc
LIBDIR  = lib
LDLIBS	= -L$(LIBDIR) -l$(LIBNAME)

all: yield

yield.o: yield.c

yield: yield.o

clean:
	$(RM) -f *.o tmr vdisk?.bin

.PHONY: clean
