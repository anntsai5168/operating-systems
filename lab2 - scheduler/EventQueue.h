#ifndef EVENTQUEUE_H
#define EVENTQUEUE_H

#include "Process.h"
#include "ProcessQueue.h"
#include "Scheduler.h"
#include "Event.h"
#include "Random.h"
#include <deque>
#include <vector>
#include <fstream>

namespace anntsai {

    class EventQueue{

        public:

            int maxPRIO;

            EventQueue(ProcessQueue *p, Random *r, int maxPRIO);

            Random *r;
            std::deque<Event> eventQueue;
            ProcessQueue *pq;

            Event * get_event();   // return pointer to Event
            void push_event(Process* evtp, int ets, transition t, stateType cs, stateType ps);
            void pop_event();
            int get_next_event_time();

    };

}

#endif