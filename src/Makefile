CC=gcc
CFLAGS=

mysh: shell.c interpreter.c shellmemory.c scheduler.c
	$(CC) $(CFLAGS) -c shell.c interpreter.c shellmemory.c scheduler.c
	$(CC) $(CFLAGS) -o mysh shell.o interpreter.o shellmemory.o scheduler.o

clean: 
	rm mysh; rm *.o


#mysh: shell.c interpreter.c shellmemory.c
#	gcc -c shell.c interpreter.c shellmemory.c
#	gcc -o mysh shell.o interpreter.o shellmemory.o

#clean: 
#	rm mysh; rm *.o
