#the C compiler
CC=gcc
CFLAGS=-g
#this rule link the object code into a executable
helloworld: helloworld.o
	$(CC) -o helloworld helloworld.o
 
#this rule is to compile the source code
helloworld.o: helloworld.c
	$(CC) $(CFLAGS) -c helloworld.c
 
clean: 
	rm -rf *.o
	rm -rf helloworld
run:
	./helloworld
