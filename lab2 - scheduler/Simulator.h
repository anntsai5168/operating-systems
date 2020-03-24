# ifndef SIMULATOR_H
# define SIMULATOR_H

# include "Process.h"
# include "EventQueue.h"
# include "Scheduler.h"
# include "Random.h"
# include <string>

namespace anntsai
{
    class Simulator{
        public:

            // Summary Info
            int lastFT;           // Finishing time of the last event
            double CPUUt;         // CPU Utilization of time at least one process is running
            double IOUt;          // IO Utilization of time at least one process is performing IO
            double avgTT;         // Average Turnaround Time Among Processes
            double avgCW;         // Average CPU Waiting Time Among Processes
            double throughput;    // Throughput of number processes per 100 time units = (total TC ) / 100

            
            Simulator(EventQueue *eq, Scheduler *sch, Random *r);

            int get_cur_proc_ts(Process *premptedProc, EventQueue *eq, int curTS);  // get timestamp
            void rm_future_event(Process *procOwnEvt, EventQueue *eq); // for preemption
    };
}

# endif