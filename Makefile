CC = gcc
CFLAGS = -Wall -Wextra -g

all: memgrind memtest 

memgrind: memgrind.c mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -o memgrind memgrind.c mymalloc.c

memtest: memtest.c mymalloc.c mymalloc.h
	$(CC) $(CFLAGS) -o memtest memtest.c mymalloc.c

clean:
	rm -f memgrind memtest *.o
