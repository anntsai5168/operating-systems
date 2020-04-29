# declare variables
CC = g++ -std=c++11
CPPFLAGS = -c -Wall
OFILES = main.o Monitor.o Pager.o Random.o

# commands
main : $(OFILES)
	$(CC) $(OFILES) -o main 

main.o: main.cpp Random.h Monitor.h 
	$(CC) $(CPPFLAGS) main.cpp

Monitor.o: Monitor.cpp Monitor.h PagerInterface.h
	$(CC) $(CPPFLAGS) Monitor.cpp

Pager.o: Pager.cpp Pager.h Random.h Process.h Structure.h
	$(CC) $(CPPFLAGS) Pager.cpp

Random.o: Random.cpp Random.h 
	$(CC) $(CPPFLAGS) Random.cpp

clean:
	rm -f main *.o *~
