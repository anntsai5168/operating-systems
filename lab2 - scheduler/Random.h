#ifndef RANDOM_H
#define RANDOM_H

#include "Process.h"
#include <string>
#include <cstring>
#include <fstream>

namespace anntsai {

    class Random {
        public:
            Random(std::string *rFile);
            ~Random();

            std::ifstream inputFile;
            std::string *rFile; 
            int rCnt;                 // total number of rfile
            int rIdx;                 // index of the number

            int myrandom(int burst);
            void generate_CPU_burst(Process *p);
            void generate_IO_burst(Process *p);

    };
}

#endif