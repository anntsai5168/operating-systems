# include "Scheduler.h"
# include <iostream>

namespace anntsai {

    Scheduler::Scheduler(): 
        maxPrio(4),    // If the addition is omitted then maxprios=4 by default
        quantum(10000) // non-preemptive schedulers have a very large quantum (e.g. 10000) which essentially means that no preemption will ever occur for those schedulers.
    {};


    // FCFS
    FCFS::FCFS(): 
        runQueue() 
    {};

    void FCFS::add_process(Process *p) {
        runQueue.push_back(p);
    }

    Process* FCFS::get_next_process() {
        if (!runQueue.empty())
        {
            Process *p = runQueue.front();
            runQueue.pop_front(); 
            return p;
        }
        return nullptr;
    }

    std::string FCFS::get_schlr_name() {
        std::string schlr = "FCFS";
        return schlr;
    }

    // LCFS
    LCFS::LCFS(): 
        runQueue() 
    {};

    void LCFS::add_process(Process *p) {
        runQueue.push_back(p);
    }

    Process* LCFS::get_next_process() {
        if (!runQueue.empty())
        {
            Process *p = runQueue.back();
            runQueue.pop_back(); 
            return p;
        }
        return nullptr;
    }

    std::string LCFS::get_schlr_name() {
        std::string schlr = "LCFS";
        return schlr;
    }

    // SRTF
    SRTF::SRTF(): 
        runQueue() 
    {};

    void SRTF::add_process(Process *p) {
        
        int idx = 0;
        while (idx < runQueue.size()) {
            if (runQueue.at(idx)->rTC > p->rTC) {
                break;
            }
            idx ++;
        }
        runQueue.insert(std::next(runQueue.begin(), idx), p);

    }

    Process* SRTF::get_next_process() {
        if (!runQueue.empty()) {
            Process *p = runQueue.front();
            runQueue.pop_front(); 
            return p;
        }
        return nullptr;
    }

    std::string SRTF::get_schlr_name() {
        std::string schlr = "SRTF";
        return schlr;
    }

    // RR
    RR::RR(int q): 
        FCFS()
    {
        quantum = q;
    };

    std::string RR::get_schlr_name() {
        std::string schlr = "RR";
        return schlr;
    }

    // PRIO
    PRIO::PRIO(int prio, int q): 
        runQueue(), 
        eprQueue(),
        runQueueLen(0), 
        eprQueueLen(0)
    {
        maxPrio = prio;
        quantum = q;

        for (int i = 0; i < maxPrio; i++){
            // utilize independent process lists at each priority level
            runQueue.push_back(std::deque<Process*>());   
            eprQueue.push_back(std::deque<Process*>());
        }

        pRunQueue = &runQueue;
        pEprQueue = &eprQueue;
    };

    void PRIO::swap(){
        std::vector< std::deque<Process*> > *tmp = pRunQueue;
        pRunQueue = pEprQueue;
        pEprQueue = tmp;
        runQueueLen = eprQueueLen;
        eprQueueLen = 0;
    }

    void PRIO::add_process(Process *p) {
        // every quantum expiration the dynamic priority decreases by one.
        if (p->state == STATE_PREEMPT){  
            p->dPRIO--;
        }

        p->state = STATE_READY;

        if (p->dPRIO < 0){
            p->dPRIO = p->PRIO - 1; // When “-1” is reached the prio is reset to (static_priority-1)
            pEprQueue->at(p->dPRIO).push_back(p);
            eprQueueLen++;
        } else {
            pRunQueue->at(p->dPRIO).push_back(p);
            runQueueLen++;
        }
    }

    Process* PRIO::get_next_process() {

        if (runQueueLen <= 0) {
            swap();
        }

        Process *p = nullptr;

        int j = -100;
        for (int i = 0; i < maxPrio; i++){
            if (!pRunQueue->at(i).empty()){
                j = i;
            }
        }
        if (j >= 0){
            p = pRunQueue->at(j).front();
            pRunQueue->at(j).pop_front();
            runQueueLen--;
        }
        return p;
    }

    std::string PRIO::get_schlr_name() {
        std::string schlr = "PRIO";
        return schlr;
    }

    // PREPRIO
    PREPRIO::PREPRIO(int prio, int q):
        PRIO(prio, q)
    {};

    std::string PREPRIO::get_schlr_name() {
        std::string schlr = "PREPRIO";
        return schlr;
    }

}