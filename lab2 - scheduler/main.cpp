#include "Process.h"
#include "ProcessQueue.h"
#include "Event.h"
#include "Random.h"
#include "EventQueue.h"
#include "Scheduler.h"
#include "SchedulerInterface.h"
#include "Simulator.h"
#include <iostream>
#include <string>
#include <cstring>
#include <queue>
#include <vector>
#include <getopt.h> // parse command-line options of the Unix/POSIX style [c lib]


using namespace anntsai;

extern int verbose;

int main(int argc, char** argv) {     // parse command input

    int inputQuantum = 0;        // command line input quantum
    int priority = 4;
    int quantum = 10000;
    
    // parse command line option
    char *svalue = nullptr;
    int c;
    opterr = 0;

    while ((c = getopt (argc, argv, "vs:")) != -1)   // ‘-1’ if there are no more options to process.
        switch (c)
        {
            case 'v': {             // v option
                verbose = 1;
            }
            case 's': {            // s option
                svalue = optarg;  // If the option takes a value, that value is pointer to the external variable optarg.
                break;
            }
            case '?': {            // ‘?’ when there is an unrecognized option and it stores into external variable optopt.
                if (optopt == 'c')
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
                else if (isprint (optopt))
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
                else
                    fprintf (stderr,
                             "Unknown option character `\\x%x'.\n",
                             optopt);
                return 1;
            }
            default: {
                abort ();
            }
        }

    // input scheduler type
    char type = svalue[0];
    schedulerType typeChar;      //  store val after transformed

    switch (type) {
        case 'F':{
            typeChar = tFCFS;
            break;
        }   
        case 'L':{
            typeChar = tLCFS;
            break;
        }
        case 'S':{
            typeChar = tSRTF;
            break;
        }
        case 'R':{
            typeChar = tRR;
            break;
        }  
        case 'P':{
            typeChar = tPRIO;
            break;
        }
        case 'E':{
            typeChar = tPREPRIO;
            break;
        }
            
    }

   

    // parse input quantum and priority
    int inputQuant = 0;
    int inputPriority = 0;
    
    char parsedStr[32];
    strcpy(parsedStr, svalue + 1);
    for (int i = 0; i < strlen(parsedStr); i++) {
        if (parsedStr[i] == ':' ) {
            parsedStr[i] = ' ';
        } 
    }
    sscanf (parsedStr,"%d %d",&inputQuant, &inputPriority);

    // update quantum and priority
    if (inputQuant > 0) {
        quantum = inputQuant;
        inputQuantum = 1;
    }

    if (inputPriority > 0) {
        priority = inputPriority;
    }
    
    // input files
    std::string inputFile = argv[optind];        // optind is for the extra arguments which are not parsed 
    std::string rFile = argv[optind + 1];

    // initiate classes
    Scheduler *schdlr = SchedulerInterface::Schlr(typeChar, priority, quantum);
    ProcessQueue inputdata(&inputFile);
    Random r(&rFile);
    EventQueue eq(&inputdata, &r, schdlr->maxPrio) ;
    Simulator sim(&eq, schdlr, &r);

    
    // print results

    // a) Scheduler information (which scheduler algorithm and in case of RR/PRIO/PREPRIO also the quantum)
    if (inputQuantum){
        printf("%s %d\n", schdlr->get_schlr_name().c_str(), quantum);
    } else {
        printf("%s\n", schdlr->get_schlr_name().c_str());
    }
  
    // b) Per process information
    for(int i = 0; i < inputdata.procQueue.size(); i++){
        Process * proc = &inputdata.procQueue.at(i);
        printf("%04d: %4d %4d %4d %4d %1d | %5d %5d %5d %5d\n",
                proc->pid ,proc->AT, proc->TC, proc->CB, proc->IO, proc->PRIO, proc->FT, proc->TT, proc->IT, proc->CW);
    }

    // c) Summary Information 
    printf("SUM: %d %.2lf %.2lf %.2lf %.2lf %.3lf\n",
            sim.lastFT, sim.CPUUt, sim.IOUt, sim.avgTT, sim.avgCW, sim.throughput);

    return 0;
};
