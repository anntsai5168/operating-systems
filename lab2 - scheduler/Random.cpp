#include "Random.h"

namespace anntsai {

    Random::Random(std::string *rFile): 
        inputFile(), 
        rFile(rFile), 
        rCnt(0), 
        rIdx(0)
    {
        std::string curLine;                    // store current parsed line
        char str[32];                           // cstring to store current parsed line
        int pid = 0;                            // Process ID

        inputFile.open(*rFile);
        getline(inputFile, curLine);
        if (inputFile.is_open()) {
            if (curLine.length() != 0) {
                strcpy(str, curLine.c_str());
                rCnt = strtol(str, NULL, 10);    // get first line
            }
        };
    }

    Random::~Random() { 
        inputFile.close(); 
    }

    // get random number
    // for the initial assignment of the static priority
    int Random::myrandom(int burst) {

        std::string curLine;
        char str[16];

        if (rIdx == rCnt){
            inputFile.close();
            inputFile.open(*rFile); // wraparound
            rIdx = 0;
            getline(inputFile, curLine);
        }

        getline(inputFile, curLine);
        strcpy(str, curLine.c_str());
        int random = strtol(str, NULL, 10);
        rIdx++;

        // return 1 + (randvals[ofs] % burst)
        return 1 + random % burst;    
    }

    // for transition 2
    void Random::generate_CPU_burst(Process *p) { 

        if (p->ranCB)  // ranCB != 0
            return;

        // cpu_burst is defined as a random number between [ 1 .. CB ]. 
        int randCB = myrandom(p->CB); 
        // If the remaining execution time is smaller than the cpu_burst compute, reduce it to the remaining execution time.
        if (randCB >= p->rTC) {
            p->ranCB = p->rTC;
        } else {
            p->ranCB = randCB; 
        }
    }

    // for transition 3
    void Random::generate_IO_burst(Process *p) { 
        p->ranIO = myrandom(p->IO); 
    }  
}