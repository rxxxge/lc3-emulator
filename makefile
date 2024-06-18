CC=gcc
CFLAGS=-std=c17 -Wall -Wextra -Werror

all:
	$(CC) lc3.c -o lc3 $(CFLAGS)