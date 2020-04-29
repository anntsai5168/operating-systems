# include "Pager.h"


// Rand
Rand::Rand(std::string *rfile):
    rd(rfile)
{};

frame_t* Rand::select_victim_frame(){
    seed = rd.getRandom(frame_table->size());
    return &frame_table->at(seed);
}


// FIFO
FIFO::FIFO():
    cur_index(0)
{};

frame_t* FIFO::select_victim_frame() {
    if (cur_index == frame_table->size()) {                         // start over
        cur_index = 0;
    }
    cur_index ++;                                                   // for next invocation
    hand = &frame_table->at(cur_index-1);
    return hand;
}


// Clock
Clock::Clock():
    cur_index(0)
{};

frame_t* Clock::select_victim_frame(){
 
   while (true){
 
       hand = &frame_table->at(cur_index);                          // cur frame
      
       if (hand->PT_pointer->VALID) {                               // check if it's valid
           if (hand->PT_pointer->REFERENCED == 0) {
               cur_index = (cur_index+1)%(frame_table->size());     // for next invocation
               return hand;
           }
           else {
               hand->PT_pointer->REFERENCED = 0;
           }
       }
       cur_index = (cur_index+1)%(frame_table->size());             // for next invocation
   }
}


// NRU
NRU::NRU(unsigned long* inst_count):
    cur_index(0),
    so_far_inst_count(inst_count),
    last_reset(0),
    classes()
{
    // initialize classes
    for (int i = 0; i < 4; i ++) {
        classes.push_back(nullptr);
    }
};

frame_t* NRU::select_victim_frame() {

    bool isReset = false;
    // update last_reset
    if (*so_far_inst_count - last_reset >= 50) {
        last_reset = *so_far_inst_count;
        isReset = true;
    }

    // reset class every invocation   
    for (int i = 0; i < 4; i ++) {
        classes.at(i) = nullptr;
    }

    // categorize & rember the first frame
    if (isReset) {
        for (int i = 0; i < frame_table->size() && isReset; i ++) {

            hand = &frame_table->at(cur_index);
            
            R = hand->PT_pointer->REFERENCED;
            M = hand->PT_pointer->MODIFIED;
            
            if (R == 0 && M == 0 && classes.at(0) == nullptr) {
                classes.at(0) = hand;
            }
            else if (R == 0 && M == 1 && classes.at(1) == nullptr) {
                classes.at(1) = hand;
            }
            else if (R == 1 && M == 0 && classes.at(2) == nullptr) {
                classes.at(2) = hand;
            }
            else if (R == 1 && M == 1 && classes.at(3) == nullptr) {
                classes.at(3) = hand;
            }

            // reset
            hand->PT_pointer->REFERENCED = (unsigned int)0;
            cur_index = (cur_index+1)%(frame_table->size());

        }
    }
    else {
        for (int i = 0; i < frame_table->size() && !isReset; i ++) {

            hand = &frame_table->at(cur_index);
            
            R = hand->PT_pointer->REFERENCED;
            M = hand->PT_pointer->MODIFIED;
            
            if (R == 0 && M == 0 && classes.at(0) == nullptr) {
                classes.at(0) = hand;
            }
            else if (R == 0 && M == 1 && classes.at(1) == nullptr) {
                classes.at(1) = hand;
            }
            else if (R == 1 && M == 0 && classes.at(2) == nullptr) {
                classes.at(2) = hand;
            }
            else if (R == 1 && M == 1 && classes.at(3) == nullptr) {
                classes.at(3) = hand;
            }

            // break
            if (classes.at(0)) {break;}
            cur_index = (cur_index+1)%(frame_table->size());

        }
    }

    // pick the victim
    for (int i = 0; i < 4; i ++) { 
        if (classes.at(i)) {    
            hand = classes.at(i);
            break;
        }
    }

    cur_index = (hand->fid + 1)%(frame_table->size()); 

    return hand;
}



// Aging
Aging::Aging():
   cur_index(0)
{};
 
frame_t* Aging::select_victim_frame() {
 
   unsigned int min = 0xffffffff;
   unsigned int cur_age;
   int min_index;
 
   // update age  [scan from the beginning]
   for (unsigned int i= 0; i < frame_table->size(); i ++) {
 
       frame_t* cur_frame;
       cur_frame = &frame_table->at(i);
       
       if (!cur_frame->PT_pointer) {continue;}
 
       cur_frame->age >>= 1;
       if(cur_frame->PT_pointer->REFERENCED){cur_frame->age = cur_frame->age| 0x80000000;}
       cur_frame->PT_pointer->REFERENCED = 0;
   }

   // pick the youngest [scan from the last min_index + 1, not from the beginning, so we need hand]

   for (unsigned int i= 0; i < frame_table->size(); i ++) {
 
       cur_age = frame_table->at(cur_index).age;

       if (cur_age < min) {                                        // we store the first min we met
           min = cur_age;
           min_index = cur_index;
           hand = &frame_table->at(min_index);
       }
       cur_index = (cur_index+1)%(frame_table->size());
   }
   //age_vector[min_index] = 0;
   cur_index = (min_index+1)%(frame_table->size());                // for next time invocation
 
   return hand;

}


// WorkingSet
WorkingSet::WorkingSet(unsigned long* inst_count):
    cur_index(0),
    so_far_inst_count(inst_count),
    TAU(49)
{};


frame_t* WorkingSet::select_victim_frame() {

    // reset hand every invocation
    hand = nullptr;

    unsigned long oldest = 0;
    int first_index = cur_index;

    //std::cout << "*so_far_inst_count " << *so_far_inst_count << std::endl;

    for (int i= 0; i < frame_table->size(); i ++) {

        frame_t* cur_frame = &frame_table->at(cur_index);

        // REFERENCED = 1
        if (cur_frame->PT_pointer->REFERENCED == 1){
            //std::cout << "here 1 " << std::endl;
            cur_frame->PT_pointer->REFERENCED = 0;
            //last_referenced_vector[cur_index] = *so_far_inst_count;
            cur_frame->last_referenced  = *so_far_inst_count;
        }
        // REFERENCED = 0
        else {
            //std::cout << "here 0 " << std::endl;
            if (*so_far_inst_count - cur_frame->last_referenced > TAU) {
                hand = cur_frame;
                break;
            }
            else {

                if ( *so_far_inst_count - cur_frame->last_referenced > oldest) {
                    hand = cur_frame;
                    oldest = *so_far_inst_count - cur_frame->last_referenced;
                }
            }
        }

        // corner case 1111 -> everybody 1-> 0 : return the first one
        if (i == frame_table->size()-1 && hand == nullptr) {
            hand = &frame_table->at(first_index);
        }
        cur_index = (cur_index+1) % (frame_table->size());
    }
    cur_index = (hand->fid +1) % (frame_table->size());
    return hand;
}
    
