#include "Simulation.h"

#include <cmath>

// initilization
Simulation::Simulation(std::string *inputfile, algorithm scheduler_type) :
   instr_vector(),
   arr_vector(),
   start_vector(),
   end_vector()
 
// body
{  
    ///////////////////////////////////////// assign pointers  ///////////////////////////////////////

    instr_vec_ptr = &instr_vector;
    io_scheduler = IOSchedulerInterface::createIOScheduler(scheduler_type);

    //////////////////////////////////////// parse input file ////////////////////////////////////////


    std::ifstream inputFile;
    std::string cur_line;
 
    inputFile.open(*inputfile);
      
    while (getline(inputFile, cur_line) && !inputFile.eof()){

          
            // ignore comments
            if (cur_line[0] != '#') {
              
                io_count ++;

                std::istringstream stream(cur_line);
                stream >> cur_ts >> cur_track;
                
                request req = {
                    .ts = cur_ts,
                    .track = cur_track,
                    .idx = idx_req
                };

                instr_vec_ptr->push_back(req);
                arr_vector.push_back(cur_ts);

                idx_req ++; 
            }

    }
    inputFile.close();

    //////////////////////////////////////// start simulation ////////////////////////////////////////

    tick = 0;
    last_track_pos = 0;
    cur_track_pos = 0;
    direction = 1;
    
    // assign start & end vector
    for (int i = 0; i < io_count; i ++) {
        start_vector.push_back(0);
        end_vector.push_back(0);
    }

    while (true) {

        // 1) If a new I/O arrived to the system at this current time
        if (instr_vec_ptr->front().ts == tick) {

            request new_req = instr_vec_ptr->front();                        // need to store, cuz the front will be poped out
            io_scheduler->enqueue(new_req);

            // tick, IO-op # (starting with 0) and the track# requested
            if (V) { printf("%d: %d add %d\n", tick,new_req.idx, new_req.track); }
        }

        
        // head is moving
        if (is_moving) {

            // 2) If an IO is active and completed at this time → Compute relevant info and store in IO request for final summary
            if (cur_track_pos == cur_req.track) {

                total_turnaround += (tick - cur_req.ts);                     // record turnaround time : submisson -> completion
                tot_movement += abs(cur_track_pos - last_track_pos);         // record total number of tracks the head had to be moved
                last_track_pos = cur_track_pos;                              // update last_track_pos

                req_proc_cnt ++;
                end_vector[cur_req.idx] = tick;

                // 1195: 18 finish 68 -> 18 is the IO-op #, 68 is total length/time of the io from request to completion
                if (V) { printf("%d: %d finish %d \n", tick, cur_req.idx, tick - cur_req.ts); }

                is_moving = false;

            }
            // 3) If an IO is active but did not yet complete → Move the head by one sector/track/unit in the direction it is going (to simulate seek)
            else {
                cur_track_pos += direction;                                  // move head by 1
            }
        }
        

        // head is not moving
        if (!is_moving) {
            // 4) If no IO request active now (after (2)) but IO requests are pending → Fetch the next request and start the new IO. 
            if (io_scheduler->io_queue.size()!=0 | io_scheduler->add_queue.size()!=0) {
                
                cur_req = io_scheduler->select_next_request();
                
                // record start time & wait time
                start_vector[cur_req.idx] = tick;
                cur_waittime = tick - cur_req.ts;
                total_waittime += cur_waittime;

                // update direction  [when cur_req.track == last_track_pos -> stay, direction remained]
                if (cur_req.track - last_track_pos > 0) {direction = 1;}      // up
                else if (cur_req.track - last_track_pos < 0) {direction = -1;}// down

                // update max waittime
                if (cur_waittime > max_waittime) {
                    max_waittime = cur_waittime;
                }
                
                // 1127: 18 issue 211 279 -> 18 is the IO-op #, 211 is the track# requested, 279 is the current track#
                if (V) { printf("%d: %d issue %d %d\n", tick, cur_req.idx, cur_req.track, cur_track_pos); }


                // start the new IO
                if (cur_req.track != last_track_pos) {                        // non-stay
                    cur_track_pos += direction;                               // move head by 1
                    is_moving = true;
                }
                else {                                                        // stay : update ending info
                    req_proc_cnt ++;                                          // processed new request in the same tick the last request completed
                    total_turnaround += (tick - cur_req.ts);                  // record turnaround time : submisson -> completion
                    end_vector[cur_req.idx] = tick;
                    if (V) { printf("%d: %d finish %d \n", tick, cur_req.idx, tick - cur_req.ts); }
                    continue;
                }
            }
            // 5) If no IO request is active now and no IO requests pending → exit simulation
            if (req_proc_cnt == io_count) {
                break;
            }
        }
        
        // 6) Increment time by 1
        tick ++;    
    }

    //////////////////////////////////////// output  ///////////////////////////////////////////////

    total_time = tick;
    avg_turnaround = total_turnaround / double(io_count);   // keep decimal
    avg_waittime = total_waittime / double(io_count);

    // per request
    for (int i = 0; i < arr_vector.size(); i ++){
        printf("%5d: %5d %5d %5d\n",i, arr_vector[i], start_vector[i], end_vector[i]);
    }
    
    // summary
    printf("SUM: %d %d %.2lf %.2lf %d\n", total_time, tot_movement, avg_turnaround, avg_waittime, max_waittime);  

}
