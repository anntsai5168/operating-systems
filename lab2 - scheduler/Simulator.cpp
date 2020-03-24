# include "Simulator.h"
# include <iostream>

int verbose = 0;                            // set global flag

namespace anntsai
{
    Simulator::Simulator(EventQueue *eq, Scheduler *sch, Random *r):
        lastFT(0),     
        CPUUt(0),         
        IOUt(0),         
        avgTT(0),           
        avgCW(0),          
        throughput(0)
    {
        Event* evt;
        Process *CURRENT_RUNNING_PROCESS;
        CURRENT_RUNNING_PROCESS = nullptr;

        bool CALL_SCHEDULER;               // recording scheduler call
        int CURRENT_TIME;
        int nextEventTime;
        int timeInPrevState;               // conditional on whether something is run

        // summary info
        double ttlCPUTime = 0;
        double ttlIOTime = 0;
        int IOProcCnt = 0;       
        int IOBeginTime = 0;              
        double ttlTTTime = 0;
        double ttlCPUWait = 0;

        while ((evt = eq->get_event())) {

            Process *proc = evt->eventProc; // this is the process the event works on
            CURRENT_TIME = evt->eventTS;
            timeInPrevState = CURRENT_TIME - proc->state_ts;

            proc->state_ts = CURRENT_TIME;

            switch (evt->trans) {          // which state to transition to? TRANS_TO_READY/ TRANS_TO_RUN / TRANS_TO_BLOCK / TRANS_TO_PREEMPT

                case TRANS_TO_READY: {
                    
                    // print out some tracing information that allows one to follow the state trans.
                    if (verbose){   
                        std::cout << " cur time : " << CURRENT_TIME << " pid : " << proc->pid << " timeInPrevState : " << timeInPrevState << ": "
                                  << "BLOCKED" << " to " << "READY" << std :: endl;
                    }

                    // must come from BLOCKED (4) or from PREEMPTION (5)
                    // 1. from BLOCKED (4)
                    if (evt->preS == STATE_BLOCKED){

                        proc->dPRIO = proc->PRIO - 1; // When a process returns from I/O its dynamic priority is reset (to (static_priority-1).
                        proc->IT += timeInPrevState;   // I/O Time ( time in STATE_BLOCKED state)

                        IOProcCnt--;
                        if (IOProcCnt == 0){ 
                            ttlIOTime += CURRENT_TIME - IOBeginTime; 
                        }   // for IOUt

                    }

                    // 2. from PREEMPTION (5)
                    // CURRENT_RUNNING_PROCESS was preempted by proc
                    if (sch->get_schlr_type() == tPREPRIO && CURRENT_RUNNING_PROCESS != nullptr){

                        if (evt->eventProc->state == STATE_BLOCKED || evt->eventProc->state == STATE_CREATED) {
                            
                            int currProcTS = get_cur_proc_ts(CURRENT_RUNNING_PROCESS, eq, CURRENT_TIME); 

                            if(verbose){
                                std::cout << "process: " << CURRENT_RUNNING_PROCESS->pid << " premmpted by " << proc->pid
                                          << " cur time :" << CURRENT_TIME << std :: endl;
                            }
                            
                            /*
                            Preemption in this case happens if the unblocking process’s dynamic priority is higher than the currently 
                            running processes dynamic priority AND the currently running process does not have an event pending for the 
                            same time stamp
                            */
                            if (((proc->dPRIO > CURRENT_RUNNING_PROCESS->dPRIO) && CURRENT_TIME < currProcTS )) {

                                if (verbose){
                                    std::cout << "preempt" << std :: endl;
                                }
                                CURRENT_RUNNING_PROCESS->PREPRIOpreempted = true;
                                /*
                                If preemption does happen, you have to remove the future event for the currently running process and 
                                add a preemption event for the current time stamp (ensure that the event is properly ordered in the eventQ). 
                                */
                                rm_future_event(CURRENT_RUNNING_PROCESS, eq);
                                // must add to run queue
                                eq->push_event(CURRENT_RUNNING_PROCESS, CURRENT_TIME, TRANS_TO_PREEMPT, STATE_READY, STATE_RUNNING);
                            } else {
                                if(verbose){
                                    std::cout << "no preempt" << std :: endl;
                                }
                            }
                        }
                    }
                    sch->add_process(proc);
                    CALL_SCHEDULER = true;
                    break;
                }
            
                case TRANS_TO_RUN: {    //  must come from READY

                    proc->state = STATE_RUNNING;

                    // When a process is scheduled (becomes RUNNING (trans 2)) the cpu_burst is defined as a random number between [ 1 .. CB ]. 
                    r->generate_CPU_burst(proc);

                    if(verbose){
                        std::string str1 = "READY";
                        std::string str2 = "RUNNING";
                        printf("%d %d %d -> %s to %s info: cb=%2d remain=%d prio=%d\n",
                               CURRENT_TIME, proc->pid, timeInPrevState, str1.c_str(), str2.c_str(), proc->ranCB, proc->rTC, proc->dPRIO + 1);    // static  = dynamic + 1
                    }
                    proc->CW += timeInPrevState;
                    ttlCPUWait += timeInPrevState;

                    // create event for either preemption or blocking
                    if(proc->ranCB <= sch->quantum){
                        eq->push_event(proc, CURRENT_TIME + proc->ranCB, TRANS_TO_BLOCK, STATE_BLOCKED, STATE_RUNNING);
                    } else {
                        eq->push_event(proc, CURRENT_TIME + sch->quantum, TRANS_TO_PREEMPT, STATE_READY, STATE_RUNNING);
                    }
                    break;
                }


                /*
                When a process finishes its current cpu_burst (assuming it has not yet reached its ttl CPU time TC), 
                it enters into a period of IO (aka BLOCKED) (trans 3) at which point the io_burst is defined as 
                a random number between [ 1 .. IO ].
                */
                case TRANS_TO_BLOCK: {  // must from running

                    // stop
                    CURRENT_RUNNING_PROCESS = nullptr;

                    proc->rTC = proc->rTC - proc->ranCB;
                    proc->ranCB -= timeInPrevState;
                    proc->ranIO = 0;

                    ttlCPUTime += timeInPrevState;

                    // if the process is completely done
                    if (proc->rTC == 0){

                        proc->state = STATE_DONE;

                        proc->FT = CURRENT_TIME;
                        proc->TT = CURRENT_TIME - proc->AT;

                        lastFT = CURRENT_TIME;
                        ttlTTTime += proc->TT;

                        if (verbose){
                            printf("%d %d %d -> Completed \n", CURRENT_TIME, proc->pid, timeInPrevState);
                        }
                    } else {   // if not yet done, enter blocked state

                        proc->state = STATE_BLOCKED;

                        if (IOProcCnt == 0)
                            IOBeginTime = CURRENT_TIME;    // reset IOBeginTime
                        IOProcCnt++;

                        r->generate_IO_burst(proc);
                        // create an event for when process becomes READY again
                        eq->push_event(proc, CURRENT_TIME + proc->ranIO, TRANS_TO_READY, STATE_READY, STATE_BLOCKED);

                        if (verbose){
                            std::string str1 = "BLOCKED";
                            std::string str2 = "READY";
                            printf("%d %d %d -> %s to %s info: iob=%2d remain=%d \n",
                                   CURRENT_TIME, proc->pid, timeInPrevState, str1.c_str(), str2.c_str(), proc->ranIO, proc->rTC);
                        }
                    }
                    CALL_SCHEDULER = true;
                    break;
                }


                case TRANS_TO_PREEMPT: {  // must from RUNNING

                    proc->rTC -= timeInPrevState;
                    proc->ranCB -= timeInPrevState;

                    if (verbose){
                        std::string str1 = "RUNNING";
                        std::string str2 = "READY";
                        printf("%d %d %d -> %s to %s info: cb=%2d remain=%d prio=%d\n",
                               CURRENT_TIME, proc->pid, timeInPrevState, str1.c_str(), str2.c_str(), proc->ranCB, proc->rTC, proc->dPRIO + 1);
                    }

                    // FROM RUNNING (2)
                    proc->state = STATE_PREEMPT;

                    // stop
                    CURRENT_RUNNING_PROCESS = nullptr;
                    ttlCPUTime += timeInPrevState;

                    // add to runqueue (no event is generated)
                    sch->add_process(proc);

                    CALL_SCHEDULER = true;
                    break;
                }
             
            }

            // current event processed done


            eq->pop_event(); // rm processed event
            evt = nullptr;

            if (CALL_SCHEDULER) {
                if (eq->get_next_event_time() == CURRENT_TIME) {  // no pending event at the same time
                    continue;                                     // process next event from Event queue
                }
                CALL_SCHEDULER = false;                           // reset global flag

                if (CURRENT_RUNNING_PROCESS == nullptr) {
                    CURRENT_RUNNING_PROCESS = sch->get_next_process();
                    
                    if (CURRENT_RUNNING_PROCESS == nullptr)
                        continue;                                 //process next event from Event queue
                    // create event to make this process runnable for same time
                    eq->push_event(CURRENT_RUNNING_PROCESS, CURRENT_TIME, TRANS_TO_RUN, STATE_RUNNING, STATE_READY);
                }
            }
        }     
        
        // processed all events

        // calculate summary info
        CPUUt = ttlCPUTime / (double) lastFT * 100.0;
        IOUt = ttlIOTime / (double) lastFT * 100.0;      
        avgTT = ttlTTTime / (double) eq->pq->procQueue.size();
        avgCW = ttlCPUWait / (double) eq->pq->procQueue.size();
        throughput = (double) eq->pq->procQueue.size() / (double) lastFT * 100.0;

    }


    // simulator body over

    // define functions

    int Simulator::get_cur_proc_ts(Process *premptedProc, EventQueue *eq, int curTS){
        
        int state_ts;

        for (int i = 0; i < eq->eventQueue.size(); i++) {
            Event * tmp = &eq->eventQueue.at(i);
            if (tmp->eventProc == premptedProc) {
                 state_ts = tmp->eventTS;
            }
        }
        return state_ts;
    }

    /*
    If preemption does happen, you have to remove the future event for the currently running process and 
    add a preemption event for the current time stamp (ensure that the event is properly ordered in the eventQ). 
    */
    void Simulator::rm_future_event(anntsai::Process *procOwnEvt, anntsai::EventQueue *eq) {

        for (int i = 0; i < eq->eventQueue.size(); i++) {
            Event * tmp = &eq->eventQueue.at(i);
            if (tmp->eventProc == procOwnEvt) {
                eq->eventQueue.erase(eq->eventQueue.begin()+i);    // vectorname.erase(position)
                return;
            }
        }
    }

}