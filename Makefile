CC = gcc
CFLAGS = -Wall -Wextra -Werror

all: print

print: print.c
	$(CC) $(CFLAGS) -o print print.c

clean:
	rm -f print