#include "Random.h"
#include "Monitor.h"
#include <iostream>
#include <getopt.h>
#include <cstring>
#include <array>
 
 
// globals variables
std::vector<frame_t>* frame_table;
std::deque<frame_t*>* free_list;
std::vector<Process>* proc_list;          // process list pointer
Process* cur_proc;
Pager* pgr;
 
// flags
bool O = false;
bool P = false;
bool F = false;
bool S = false;
 
//////////////////////////////////////// MAIN ////////////////////////////////////////
 
// ./mmu -f4 –ac –oOPFS infile rfile    0 1:frame size 2:algo 3:option 4:infile 5:rfile
 
int main(int argc, char** argv) {
  
   int FT_size;            // 1
   algorithm pgr_type;     // 2
 
   std::string inputFile;  // 4 "in1"
   std::string rFile;      // 5 "rfile"
 
   // getOpt
   int c;
   int optLen;
   while ((c = getopt(argc, argv, "a:o:f:")) != -1) {
       switch (c) {
           // set algorithm chosen
           case 'a':
               //avalue = optarg;
               switch (optarg[0]) {
                   case 'f': pgr_type = FIFO; break;
                   case 'r': pgr_type = Rand; break;
                   case 'c': pgr_type = Clock; break;
                   case 'e': pgr_type = NRU; break;
                   case 'a': pgr_type = Aging; break;
                   case 'w': pgr_type = WorkingSet; break;
                   default:  break;
               }
               break;
           // set printing options
           case 'o':
               //ovalue = optarg;
               optLen = strlen(optarg);
               for (int i = 0; i < optLen; ++i) {
                   switch (optarg[i]) {
                       case 'O': O = true; break;
                       case 'P': P = true; break;
                       case 'F': F = true; break;
                       case 'S': S = true; break;
                   }
               }
               break;
           // set number of frames
           case 'f':
               sscanf(optarg, "%d", &FT_size);
               break;
           case '?':          // ‘?’ when there is an unrecognized option and it stores into external variable optopt.
               if (optopt == 'c')
                   fprintf (stderr, "Option -%c requires an argument.\n", optopt);
               else if (isprint (optopt))
                   fprintf (stderr, "Unknown option `-%c'.\n", optopt);
               else
                   fprintf (stderr,
                            "Unknown option character `\\x%x'.\n",
                            optopt);
               return 1;
           default:
               abort ();
       }
   }
 
   //////// frame table & free list ////////
 
   // create frame table
   frame_t fte;
   std::vector<frame_t> frame_table_obj;
 
   for (int i = 0; i < FT_size; i++) {
 
       fte.fid = i;
       fte.pid = -1;
       fte.PT_pointer = nullptr;
       fte.age = 0;
       fte.last_referenced = 0;
 
       frame_table_obj.push_back(fte);
      
   }
   // point to frame table
   frame_table = &frame_table_obj;
 
   // create free list
   std::deque<frame_t*> free_list_obj;
 
   for (int i = 0; i < FT_size; i++) {
       free_list_obj.push_back(&frame_table->at(i));
   }
   // point to free list
   free_list = &free_list_obj;
 
   //////////////////////////////////////
 
   // inputfile & rfile
   inputFile = argv[argc - 2];
   rFile = argv[argc - 1];
 
   // start simulation
   Monitor monitor = Monitor(pgr_type, &inputFile, FT_size, &rFile);
  
   return 0;
 
};
