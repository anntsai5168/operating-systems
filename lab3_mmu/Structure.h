/* all structures*/

// page table entry [initialized with 0]
struct PTE {   
    unsigned int VALID:1;
    unsigned int WRITE_PROTECT:1;
    unsigned int MODIFIED:1;
    unsigned int REFERENCED:1;
    unsigned int PAGEDOUT:1;        // has beem swapped out or not
    unsigned int vaddr:6;           // vpage index
    unsigned int frame_index:7;     // the frame this pte point to
    unsigned int isVMA:1;           // assigned to a VMA or not
    unsigned int isFilemapped:1;    // filemapped or not

    PTE(): 
        VALID(0), 
        WRITE_PROTECT(0), 
        MODIFIED(0), 
        REFERENCED(0), 
        PAGEDOUT(0),
        vaddr(0),
        frame_index(0), 
        isVMA(0),
        isFilemapped(0)
    {}
};

struct VMA {
    int start_vpage;
    int end_vpage;
    int write_protected;
    int file_mapped;
};


// statistics per process
struct Statistics {           
    unsigned long unmaps;
    unsigned long maps;
    unsigned long ins;
    unsigned long outs;
    unsigned long fins;
    unsigned long fouts;
    unsigned long zeros;
    unsigned long segv;
    unsigned long segprot;

    Statistics(): 
        unmaps(0), 
        maps(0), 
        ins(0), 
        outs(0), 
        fins(0),
        fouts(0), 
        zeros(0), 
        segv(0),
        segprot(0)
    {}
};

// frame table entry
struct frame_t {                              
    int fid;                              // frame id
    int pid;                              // process id
    PTE *PT_pointer;                      // point to PTE
    unsigned int age;                     // for Aging
    unsigned long last_referenced;        // for Working set
};