# ifndef PROCESS_H
# define PROCESS_H

namespace anntsai {

    enum stateType {
        STATE_CREATED, STATE_READY, STATE_RUNNING, STATE_BLOCKED, STATE_PREEMPT, STATE_NONE, STATE_DONE
    };

    class Process {
        public:
            int pid;  // process ID
            int AT;   // Arrival Time
            int TC;   // Total CPU Time
            int CB;   // CPU Burst
            int IO;   // IO Burst
            int PRIO; // Static Priority
            int FT;   // Finishing Time
            int TT;   // Turnaround time ( finishing time - AT )
            int IT;   // I/O Time ( time in blocked state)
            int CW;   // CPU Waiting time ( time in Ready state )

            stateType state;
            bool PREPRIOpreempted;   // be preempted by PREPRIO or not
            int dPRIO;       // dynamic priority
            int rTC;         // remaining CPU time
            int ranCB;       // CPU burst generated from random
            int ranIO;       // IO burst generated from random
            int state_ts;          // timestamp

            Process(int pid, int AT, int TC, int CB, int IO);
        
    };
}

# endif

