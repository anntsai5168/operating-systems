#ifndef PAGER_H
#define PAGER_H

# include "Random.h"
# include "Process.h"

# include <array>
# include <iostream>


extern std::vector<frame_t>* frame_table;
extern Process* cur_proc;

class Pager {
    public : 
        
        virtual frame_t* select_victim_frame() = 0;    

};


class Rand: public Pager{
    public:

        Random rd;
        int seed;

        Rand(std::string *rfile);

        frame_t* select_victim_frame() override;
};

class FIFO: public Pager {

    public:

        FIFO();

        int cur_index;                    // the index of the frame pointing to
        frame_t* hand;

        frame_t* select_victim_frame() override;
};

class Clock: public FIFO { 
 
   public:
      
       Clock();
 
       int cur_index;
       frame_t* hand;
       unsigned int frame_index;
 
       frame_t* select_victim_frame() override;
};



// If 50 or more instructions have passed since the last time the reference bits were reset, 
// then the reference bit of the pte’s reached by the frame traversal should be cleared
class NRU: public Pager {

    public:
        
        //NRU();
        NRU(unsigned long* inst_count);

        int cur_index;
        frame_t* hand;                                // hand->fid

        unsigned long* so_far_inst_count;
        unsigned long last_reset;                     // last time reset reference bit
        int class_i;                                  // which class [0~3] : (0, 0), (0, 1), (1, 0), (1, 1)
        unsigned int R;                               // reference bit
        unsigned int M;                               // modification bit

        std::vector<frame_t*> classes;                // classes is pointing to the array of vectors of frame_t of size 4
        
       
        frame_t* select_victim_frame() override;
};



class Aging: public Pager {
 
  
   public:
      
       Aging();
 
       int cur_index;   
       frame_t* hand;
 
       frame_t* select_victim_frame() override;
};



class WorkingSet: public Pager {
    
    public:
        
        WorkingSet(unsigned long* inst_count);

        int cur_index;
        frame_t* hand;

        unsigned long* so_far_inst_count;
        int TAU;

        //std::vector<unsigned long> last_referenced_vector;

        frame_t* select_victim_frame() override;
};

#endif
