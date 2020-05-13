#ifndef IOSCHEDULERINTERFACE_H
#define IOSCHEDULERINTERFACE_H

#include "IOScheduler.h"


extern std::vector<request>* instr_vec_ptr;

enum algorithm {
        FIFO,
        SSTF,
        LOOK,
        CLOOK,
        FLOOK
};

class IOSchedulerInterface {

    public:

        static IOScheduler* createIOScheduler(algorithm scheduler_type) {
            switch (scheduler_type)
            {
                case FIFO:
                    return new class FIFO();
                case SSTF:
                    return new class SSTF();
                case LOOK:
                    return new class LOOK();
                case CLOOK:
                    return new class CLOOK();
                case FLOOK:
                    return new class FLOOK();
                default:
                    return nullptr;
            }
        }
};

#endif