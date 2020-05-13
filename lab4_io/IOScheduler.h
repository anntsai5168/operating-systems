#ifndef IOSCHEDULER_H
#define IOSCHEDULER_H

#include "Structures.h"

#include <iostream>
#include <vector>
#include <deque>


extern std::vector<request>* instr_vec_ptr;

extern int tick;                                            // current time [calculate ttl time]
extern int last_track_pos;                                  // last track
extern int cur_track_pos;                                   // current track
extern int direction;                                       // moving direction : 1 / -1

extern bool is_moving;                                      // indicator: head is moving or not

class IOScheduler {
    public : 
        IOScheduler();

        std::deque<request> io_queue;                       // aka active queue
        std::deque<request> add_queue;                      // add new io requests
        std::deque<request>* ptr;                           // queue pointer, 找不到再改去 flook

        virtual request select_next_request() = 0;
        virtual void enqueue(request new_req) = 0;          // queue: io-queue / add-queue
        virtual std::string get_type() = 0;

};

class FIFO: public IOScheduler{
    public:

        FIFO();

        request select_next_request() override;
        void enqueue(request new_req) override;
        std::string get_type() override {return "FIFO";}
};

class SSTF: public IOScheduler{
    public:

        SSTF();

        request select_next_request() override;
        void enqueue(request new_req) override;
        std::string get_type() override {return "SSTF";}
};

class LOOK: public IOScheduler{
    public:

        LOOK();

        request select_next_request() override;
        void enqueue(request new_req) override;
        std::string get_type() override {return "LOOK";}
};

class CLOOK: public IOScheduler{
    public:

        CLOOK();

        request select_next_request() override;
        void enqueue(request new_req) override;
        std::string get_type() override {return "CLOOK";}
};

class FLOOK: public IOScheduler{
    public:

        FLOOK();

        std::deque<request>* queue_ptr;

        request select_next_request() override;
        void enqueue(request new_req) override;
        std::string get_type() override {return "FLOOK";}

        void swap();                                          // swap io-queue and add-queue
};

#endif