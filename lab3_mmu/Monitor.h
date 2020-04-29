/* read input */

#ifndef MONITOR_H
#define MONITOR_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <vector>
#include <deque>

#include "PagerInterface.h"




class Monitor {

    public:
        
        Monitor(algorithm pgr_type, std::string *file, int size, std::string *rfile);

        int proc_num = 0;                                 // ttl number of processes
        int pid = 0;                                      // process ID
        int vmas;
        
        std::vector<Process> proc_list_obj;               // physical process list

        // process info --> push into VMA list
        int start_vpage_in;  
        int end_vpage_in;
        int write_protected_in;
        int file_mapped_in;

        // instructions
        char instr;                                       // store instruction
        int vpage_num;                                    // store virtual page number

        // pointers
        Statistics *pstats;                               // point to current process statistics
        PTE * pte;                                        // point to cur instr's PTE;
        
        // statistics - summary
        unsigned long inst_count = 0;
        unsigned long ctx_switches = 0;                   // c
        unsigned long process_exits = 0;                  // e
        unsigned long long cost = 0;

        // functions
        frame_t *allocate_frame_from_free_list();
        frame_t *get_frame();
        void process_exit(Process * cur_proc);

        void set_up_pte(Process * cur_procm, PTE* pte);   // set up vma info
        void update_r_and_m_bits(Process * cur_proc, char instr, PTE* pte);   // update R, M bits
      
      
        void map(Process * cur_proc, frame_t * frame, PTE* pte, Pager* pgr);
        void unmap(frame_t * frame);
         
};


#endif
