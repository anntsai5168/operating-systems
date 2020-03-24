#ifndef EVENT_H
#define EVENT_H

#include "Process.h"

namespace anntsai {

    enum transition { 
        TRANS_TO_READY, TRANS_TO_RUN, TRANS_TO_BLOCK, TRANS_TO_PREEMPT 
    };

    class Event {
        public:
            Process* eventProc;
            int eventTS; // timestamp
            transition trans;
            stateType curS; // current state
            stateType preS; // previous state

            Event(Process* evtp, int ets, transition t, stateType cs, stateType ps){
                eventProc = evtp;
                eventTS = ets;
                trans = t;
                curS = cs;
                preS = ps;
            }
    };

}

#endif