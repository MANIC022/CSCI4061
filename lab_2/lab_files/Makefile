# the compiler: gcc for C program, define as g++ for C++
CC = gcc

CFLAGS  = -g -Wall

all: fork orphan wait wait_error zombie

fork: fork.c
	$(CC) $(CFLAGS) -o fork fork.c
orphan: orphan.c
	$(CC) $(CFLAGS) -o orphan orphan.c
wait: wait.c
	$(CC) $(CFLAGS) -o wait wait.c
wait_error: wait_error.c
	$(CC) $(CFLAGS) -o wait_error wait_error.c
zombie: zombie.c
	$(CC) $(CFLAGS) -o zombie zombie.c
clean:
	$(RM) -f fork orphan wait wait_error zombie
