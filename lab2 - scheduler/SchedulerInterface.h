#ifndef SCHEDULERINTERFACE_H
#define SCHEDULERINTERFACE_H

#include "Scheduler.h"

namespace anntsai {

    class SchedulerInterface { 
        
        public:

            static Scheduler *Schlr(schedulerType type, int prio, int q) {
                
                switch(type) {
                    case(tFCFS): {
                        return new FCFS();
                    }
                    case(tLCFS): {
                        return new LCFS();
                    }
                    case(tSRTF): {
                        return new SRTF();
                    }
                    case(tRR): {
                        return new RR(q);
                    } 
                    case(tPRIO): {
                        return new PRIO(prio, q);
                    }
                    case(tPREPRIO): {
                        return new PREPRIO(prio, q);
                    }
                    default: {
                        return nullptr;
                    }
                }

            }
    };
}

#endif