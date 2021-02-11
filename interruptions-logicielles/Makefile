LIBNAME=hardware

RM	= rm
CC	= gcc
LIBDIR  = lib
LDLIBS	= -L$(LIBDIR) -l$(LIBNAME)

BINARIES= mi_kernel
OBJECTS	= $(addsuffix .o,\
	   mi_kernel mi_user)
all: $(BINARIES) $(OBJECTS)

mi_user : mi_user.o $(OBJECTS)
mi_kernel : mi_kernel.o $(OBJECTS)

mi_user.o: mi_user.c
mi_kernel.o: mi_kernel.c 

clean:
	$(RM) -f *.o tmr vdisk?.bin

