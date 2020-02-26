# declare variables
CC = g++ -std=c++11
CPPFLAGS = -c -Wall

# commands
linker: linker.o
	$(CC) linker.o -o linker

linker.o: linker.cpp
	$(CC) $(CPPFLAGS) linker.cpp 

clean:
	rm -f linker *.o *~
