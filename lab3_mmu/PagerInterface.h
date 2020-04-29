#ifndef PAGERINTERFACE_H
#define PAGERINTERFACE_H

#include "Pager.h"

enum algorithm {
        Rand,
        FIFO,
        Clock,
        NRU,
        Aging,
        WorkingSet
};

class PagerInterface {

    public:

        static Pager* createPager(algorithm pgr_type, std::string *rfile, unsigned long* inst_count) {
            switch (pgr_type)
            {
                case Rand:
                    return new class Rand(rfile);
                case FIFO:
                    return new class FIFO();
                case Clock:
                    return new class Clock();
                case NRU:
                    return new class NRU(inst_count);
                case Aging:
                    return new class Aging();
                case WorkingSet:
                    return new class WorkingSet(inst_count);
                default:
                    return nullptr;;
            }
        }
};

#endif