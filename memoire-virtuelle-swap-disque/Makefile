# $Id: Makefile 167 2015-01-23 17:51:37Z phm $
##############################################################################
ROOTDIR=../
LIBNAME=hardware

RM	= rm
CC	= gcc
LIBDIR  = lib
INCDIR  = include
CFLAGS	= -Wall -ansi -pedantic -I$(INCDIR)
LDLIBS	= -L$(LIBDIR) -l$(LIBNAME)

all: swap

swap.o: swap.c

swap: swap.o

clean:
	$(RM) -f *.o swap vdisk?.bin

.PHONY: clean
