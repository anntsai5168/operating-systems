# ifndef SCHEDULER_H
# define SCHEDULER_H

# include "Process.h"
# include "string"
# include "deque"
# include "vector"

namespace anntsai {

    enum schedulerType {
        tFCFS, tLCFS, tSRTF, tRR, tPRIO, tPREPRIO
    };

    // base class
    class Scheduler {
        
        public:
            
            int maxPrio;
            int quantum;
            
            Scheduler();

            // virtula functions to be overwritten
            virtual void add_process(Process *p) = 0;
            virtual Process* get_next_process() = 0;
            virtual std::string get_schlr_name() = 0;
            virtual schedulerType get_schlr_type() = 0;
    };

    // FCFS
    class FCFS: public Scheduler {
        public:

            schedulerType type = tFCFS;
            std::deque<Process*> runQueue;

            FCFS();

            virtual void add_process(Process *p) override;
            virtual Process* get_next_process() override;
            virtual std::string get_schlr_name() override;
            // inline function
            virtual schedulerType get_schlr_type() override {
                return tFCFS;
            };
    };   

    // LCFS
    class LCFS: public Scheduler {
        public:

            schedulerType type = tLCFS;
            std::deque<Process*> runQueue;

            LCFS();

            virtual void add_process(Process *p) override;
            virtual Process* get_next_process() override;
            virtual std::string get_schlr_name() override;
            virtual schedulerType get_schlr_type() override {
                return tLCFS;
            };
    }; 

    // SRTF
    class SRTF: public Scheduler {
        public:

            schedulerType type = tSRTF;
            std::deque<Process*> runQueue;

            SRTF();

            virtual void add_process(Process *p) override;
            virtual Process* get_next_process() override;
            virtual std::string get_schlr_name() override;
            virtual schedulerType get_schlr_type() override {
                return tSRTF;
            };
    };

    // RR
    class RR: public FCFS {
        public:

            schedulerType type = tRR;

            RR(int quantum);

            virtual std::string get_schlr_name() override;
            virtual schedulerType get_schlr_type() override {
                return tRR;
            };
    };  

    // PRIO
    class PRIO: public Scheduler {
        public:

            schedulerType type = tPRIO;
            
            // declare queue
            std::vector< std::deque<Process*> > runQueue;
            std::vector< std::deque<Process*> > eprQueue; // expired queue
            // declare pointer
            std::vector< std::deque<Process*> > *pRunQueue = &runQueue;
            std::vector< std::deque<Process*> > *pEprQueue = &eprQueue;
            // length of queue
            int runQueueLen;
            int eprQueueLen;

            PRIO(int maxPrio, int quantum);

            void swap();  // swap run queue and expired queue
            virtual void add_process(Process *p) override;
            virtual Process* get_next_process() override;
            virtual std::string get_schlr_name() override;
            virtual schedulerType get_schlr_type() override {
                return tPRIO;
            };
    };   

    // PREPRIO
    class PREPRIO: public PRIO {
        public:
            PREPRIO(int maxPrio, int quantum);

            virtual std::string get_schlr_name() override;
            virtual schedulerType get_schlr_type() override {
                return tPREPRIO;
            };
    };       
    
}

# endif
