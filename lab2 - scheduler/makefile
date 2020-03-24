# declare variables
CC = g++ -std=c++11
CPPFLAGS = -c -Wall
OFILES = main.o Simulator.o EventQueue.o Random.o Event.o SchedulerInterface.o Scheduler.o ProcessQueue.o Process.o

# commands
main : $(OFILES)
	$(CC) $(OFILES) -o main 

main.o: main.cpp Simulator.h EventQueue.h Random.h Event.h SchedulerInterface.h Scheduler.h ProcessQueue.h Process.h
	$(CC) $(CPPFLAGS) main.cpp

Simulator.o: Simulator.cpp Simulator.h  EventQueue.h Random.h Scheduler.h Process.h
	$(CC) $(CPPFLAGS) Simulator.cpp

EventQueue.o: EventQueue.cpp EventQueue.h Random.h Event.h Scheduler.h ProcessQueue.h Process.h 
	$(CC) $(CPPFLAGS) EventQueue.cpp

Random.o: Random.cpp Random.h Process.h
	$(CC) $(CPPFLAGS) Random.cpp

Event.o: Event.cpp Event.h Process.h
	$(CC) $(CPPFLAGS) Event.cpp

SchedulerInterface.o: SchedulerInterface.cpp SchedulerInterface.h Scheduler.h
	$(CC) $(CPPFLAGS) SchedulerInterface.cpp

Scheduler.o: Scheduler.cpp Scheduler.h Process.h
	$(CC) $(CPPFLAGS) Scheduler.cpp

ProcessQueue.o: ProcessQueue.cpp ProcessQueue.h Process.h
	$(CC) $(CPPFLAGS) ProcessQueue.cpp

Process.o: Process.cpp Process.h 
	$(CC) $(CPPFLAGS) Process.cpp

clean:
	rm -f main *.o *~
