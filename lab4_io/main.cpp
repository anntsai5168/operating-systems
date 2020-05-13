#include "Simulation.h"

#include <iostream>
#include <getopt.h>

// globals
std::vector<request>* instr_vec_ptr;              // instruction vector pointer

int tick;                                         // current time [calculate ttl time]
int last_track_pos;                               // last track
int cur_track_pos;                                // current track
int direction;                                    // moving direction : 1 / -1

bool is_moving = false;                           // indicator: head is moving or not
bool V = false;

//////////////////////////////////////// MAIN ////////////////////////////////////////
 
// ./iosched –s<schedalgo> [options] <inputfile>
 
int main(int argc, char** argv) {
    
    std::string inputFile;
    algorithm scheduler_type; 

    // inputfile
    inputFile = argv[argc - 1];
        
    // parse command line input
    int c;

    while ((c = getopt(argc, argv, "v:s:")) != -1) {
        switch (c) {
            // set algorithm chosen
            case 's':
                switch (optarg[0]) {
                    case 'i': scheduler_type = FIFO; break;
                    case 'j': scheduler_type = SSTF; break;
                    case 's': scheduler_type = LOOK; break;
                    case 'c': scheduler_type = CLOOK; break;
                    case 'f': scheduler_type = FLOOK; break;
                    default:  break;
                }
                break;
            // set flags
            case 'v':
                V = true;
                break;
            // ‘?’ when there is an unrecognized option and it stores into external variable optopt.
            case '?':          
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

    // start simulation
    Simulation sm = Simulation(&inputFile, scheduler_type);
  
    return 0;

}
