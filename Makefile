CC=gcc
CFLAGS= -g

all:try display_stack

display_stack: display_stack.c 
	$(CC) $(CFLAGS) -o $@ $^

try: try.c
	$(CC) $(CFLAGS) -o $@  $^
	
clean:
	rm -f *.o 
	rm display_stack 
	rm try



