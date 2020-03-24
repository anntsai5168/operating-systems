#include "EventQueue.h"


namespace anntsai {

    EventQueue::EventQueue(ProcessQueue *p, Random *r, int maxPRIO) :
        pq(p),
        r(r),
        maxPRIO(maxPRIO),
        eventQueue()
    {
        for (int i = 0; i < pq->procQueue.size(); i++){

            eventQueue.push_back( Event( &pq->procQueue.at(i), pq->procQueue.at(i).AT, TRANS_TO_READY, STATE_READY, STATE_CREATED) );
    
            int staticPrio = r->myrandom(maxPRIO);      // When reading the process specification at program start, always assign a static_priority to the process using myrandom(maxprio) 
            pq->procQueue.at(i).PRIO = staticPrio;
            pq->procQueue.at(i).dPRIO = staticPrio - 1; // When “-1” is reached the prio is reset to (static_priority-1). 
        }
    }

    void EventQueue::push_event(Process* evtp, int ets, transition t, stateType cs, stateType ps) {

        int idx = 0;
        while(idx < eventQueue.size()){
            if (eventQueue.at(idx).eventTS > ets)
                break;
            idx++;
        }

        eventQueue.insert(std::next(eventQueue.begin(),idx), Event(evtp, ets, t, cs, ps));
    }

    // pointer to next event
    Event* EventQueue::get_event()
    {
        if (!eventQueue.empty()) {
            return &eventQueue.front();
        } else {
            return nullptr;
        }
    }

    // remove event
    void EventQueue::pop_event()
    {
        if (!eventQueue.empty()) {
            eventQueue.pop_front();
        }
    }

    int EventQueue::get_next_event_time()
    {
        if (!eventQueue.empty()) {
            return eventQueue.front().eventTS;
        } else {
            return 0;
        }
    }
}