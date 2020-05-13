#ifndef SIMULATION_H
#define SIMULATION_H

#include "IOSchedulerInterface.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <deque>



extern std::vector<request>* instr_vec_ptr;

extern int tick;                                  // current time [calculate ttl time]
extern int last_track_pos;                        // last track
extern int cur_track_pos;                         // current track
extern int direction;                             // moving direction : 1 / -1

extern bool is_moving;                            // indicator: head is moving or not
extern bool V;

////////////////////////////////////////////////////////

class Simulation {

    public:

        Simulation(std::string *inputfile, algorithm scheduler_type);

        int io_count = 0;                        // # of io request
        int req_proc_cnt = 0;                    // record the # of processed request
        int idx_req = 0;                         // index of io request

        int cur_ts;                              // current input timestamp
        int cur_track;                           // current input track

        request cur_req;                         // current processing request

        // pointers
        IOScheduler *io_scheduler;               // point yo io scheduler

        // vectors
        std::vector<request> instr_vector;       // store instructions
        std::vector<int> arr_vector;             // arrival time
        std::vector<int> start_vector;           // start time
        std::vector<int> end_vector;             // end time

        // summary info
        int total_time = 0;                      // total simulated time, i.e. until the last I/O request has completed.
        int tot_movement = 0;                    // total number of tracks the head had to be moved
        double avg_turnaround = 0;               // average turnaround time per operation from time of submission to time of completion
        double avg_waittime = 0;                 // average wait time per operation (time from submission to issue of IO request to start disk operation) 
        int max_waittime = 0;                    // maximum wait time for any IO operation.
        int cur_waittime = 0;
        int total_waittime = 0;
        int total_turnaround = 0;

};



#endif