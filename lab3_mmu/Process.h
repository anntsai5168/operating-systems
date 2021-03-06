#ifndef PROCESS_H
#define PROCESS_H


#include <deque>
#include <array>
#include <vector>

#include "Structure.h"

#define PT_size 64 // declare page table size as constant  

class Process {
    
    public:
        
        // structure
        int vmas;                      
        int pid;                       // process id
        VMA* cur_vma;
        Statistics statistics;
        std::array<PTE, PT_size> PT;   // page table
        std::vector<VMA> vma_list;     // vma list
        
        Process(int vs, int idx) {
                vmas = vs;
                pid = idx;
        }
};

#endif
