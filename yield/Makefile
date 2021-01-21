# $Id: Makefile 167 2015-01-23 17:51:37Z phm $
##############################################################################

LIBNAME=hardware

RM	= rm
CC	= gcc
LIBDIR  = lib
INCDIR  = include
LDLIBS	= -L$(LIBDIR) -l$(LIBNAME)

all: yield

yield.o: yield.c

yield: yield.o

clean:
	$(RM) -f *.o tmr vdisk?.bin

.PHONY: clean
