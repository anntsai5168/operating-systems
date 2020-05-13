# declare variables
CC = g++ -std=c++11
CPPFLAGS = -c -Wall
OFILES = main.o Simulation.o IOScheduler.o

# commands
iosched : $(OFILES)
	$(CC) $(OFILES) -o iosched 

main.o: main.cpp Simulation.h 
	$(CC) $(CPPFLAGS) main.cpp

Simulation.o: Simulation.cpp Simulation.h IOSchedulerInterface.h
	$(CC) $(CPPFLAGS) Simulation.cpp

IOScheduler.o: IOScheduler.cpp IOScheduler.h Structures.h 
	$(CC) $(CPPFLAGS) IOScheduler.cpp

clean:
	rm -f iosched *.o *~
