#include "IOScheduler.h"


// IOScheduler
IOScheduler::IOScheduler():
    io_queue(),
    add_queue(),
    ptr(&io_queue)
{};


// FIFO
FIFO::FIFO()
{};

request FIFO::select_next_request() {
    request next = io_queue.front();
    io_queue.erase(io_queue.begin());
    return next;
}

void FIFO::enqueue(request new_req) {
    io_queue.push_back(new_req);
    instr_vec_ptr->erase(instr_vec_ptr->begin());
}


// SSTF
SSTF::SSTF()
{};

request SSTF::select_next_request() {
    
    request next;
    int min = 2147483647;
    int io_queue_idx;

    for (int i = 0; i < io_queue.size(); i++) {
        
        int cur_dis = abs(io_queue[i].track - last_track_pos);
        
        if (cur_dis < min) {  
            io_queue_idx = i;
            min = cur_dis;
            next = io_queue[i];
        }
    }
    io_queue.erase(io_queue.begin() + io_queue_idx);

    return next;
}

void SSTF::enqueue(request new_req) {
    io_queue.push_back(new_req);
    instr_vec_ptr->erase(instr_vec_ptr->begin());
}


// LOOK
LOOK::LOOK()
{};

request LOOK::select_next_request() {

    request next;
 
    int min = 2147483647;
    int io_queue_idx = -1;
 
    for (int i = 0; i < io_queue.size(); i++) {
 
        // direction
        int cur_direction = 0;                                                       // stay
        if (io_queue[i].track - last_track_pos > 0) {
            cur_direction = 1;
        }
        else if (io_queue[i].track - last_track_pos < 0) {
            cur_direction = -1;
        }
 
        // distance
        int cur_dis = abs(io_queue[i].track - last_track_pos);
        if (cur_dis < min && cur_direction == direction | cur_direction == 0) {      // only consider same direction / stay
          
            io_queue_idx = i;
            min = cur_dis;
            next = io_queue[i];
       }
   }
 
    // change direction
    if (io_queue_idx == -1) {
 
        for (int i = 0; i < io_queue.size(); i++) {
 
            // direction
            int cur_direction = 0;                                                   // stay
            if (io_queue[i].track - last_track_pos > 0) {
                cur_direction = 1;
            }
            else {
                cur_direction = -1;
            }
 
            // distance
            int cur_dis = abs(io_queue[i].track - last_track_pos);
            if (cur_dis < min && cur_direction != direction) {                       // only consider same direction
              
                io_queue_idx = i;
                min = cur_dis;
                next = io_queue[i];
            }
        }
    }
    io_queue.erase(io_queue.begin() + io_queue_idx);
 
    return next;
}

void LOOK::enqueue(request new_req) {
    io_queue.push_back(new_req);
    instr_vec_ptr->erase(instr_vec_ptr->begin());
}


// CLOOK
CLOOK::CLOOK()
{};

request CLOOK::select_next_request() {

    request next;
 
    int min = 2147483647;
    int io_queue_idx = -1;
 
    for (int i = 0; i < io_queue.size(); i++) {
 
        // direction
        int cur_direction = 0;                                                       // stay
        if (io_queue[i].track - last_track_pos > 0) {
            cur_direction = 1;
        }
        else if (io_queue[i].track - last_track_pos < 0) {
            cur_direction = -1;
        }
 
        // distance
        int cur_dis = abs(io_queue[i].track - last_track_pos);
        
        if (cur_dis < min && cur_direction == 1 | cur_direction == 0) {             // only consider up / stay  
            io_queue_idx = i;
            min = cur_dis;
            next = io_queue[i];
        }
    }
 
    // return to the track which is the nearest to the start [track 0]
    if (io_queue_idx == -1) {

        int min_track = 2147483647;

        for (int i = 0; i < io_queue.size(); i++) {

            if (io_queue[i].track < min_track) { 
                io_queue_idx = i;
                min_track = io_queue[i].track;
                next = io_queue[i];
            }
        }
    }
    io_queue.erase(io_queue.begin() + io_queue_idx);
 
    return next;
}

void CLOOK::enqueue(request new_req) {
    io_queue.push_back(new_req);
    instr_vec_ptr->erase(instr_vec_ptr->begin());
}


// FLOOK
FLOOK::FLOOK():
    queue_ptr(&io_queue)
{};

request FLOOK::select_next_request() {

    // swap
    if (queue_ptr->size() == 0) {
        swap();
    }
    
    request next;
 
    int min = 2147483647;
    int io_queue_idx = -1;
 
    for (int i = 0; i < queue_ptr->size(); i++) {
 
        // direction
        int cur_direction = 0;                                                       // stay
        if (queue_ptr->at(i).track - last_track_pos > 0) {
            cur_direction = 1;
        }
        else if (queue_ptr->at(i).track - last_track_pos < 0) {
            cur_direction = -1;
        }
 
        // distance
        int cur_dis = abs(queue_ptr->at(i).track - last_track_pos);
        if (cur_dis < min && cur_direction == direction | cur_direction == 0) {      // only consider same direction / stay
          
            io_queue_idx = i;
            min = cur_dis;
            next = queue_ptr->at(i);
       }
   }
 
    // change direction
    if (io_queue_idx == -1) {
 
        for (int i = 0; i < queue_ptr->size(); i++) {
 
            // direction
            int cur_direction = 0;                                                   // stay
            if (queue_ptr->at(i).track - last_track_pos > 0) {
                cur_direction = 1;
            }
            else {
                cur_direction = -1;
            }
 
            // distance
            int cur_dis = abs(queue_ptr->at(i).track - last_track_pos);
            if (cur_dis < min && cur_direction != direction) {                       // only consider same direction
              
                io_queue_idx = i;
                min = cur_dis;
                next = queue_ptr->at(i);
            }
        }
    }
    queue_ptr->erase(queue_ptr->begin() + io_queue_idx);
 
    return next;

}

void FLOOK::enqueue(request new_req) {

    if (is_moving) {

        if (queue_ptr->begin() == io_queue.begin()) {
            add_queue.push_back(new_req);
        }
        else if (queue_ptr->begin() == add_queue.begin()) {
            io_queue.push_back(new_req);
        }
    }
    else {
        queue_ptr->push_back(new_req);
    }
    
    instr_vec_ptr->erase(instr_vec_ptr->begin());
}

void FLOOK::swap() {

    if (queue_ptr->begin() == io_queue.begin()) {
        queue_ptr = &add_queue;
    }
    else if (queue_ptr->begin() == add_queue.begin()) {
        queue_ptr = &io_queue;
    }

}