# include "ProcessQueue.h"
# include <cstring>

namespace anntsai {
    // init
    ProcessQueue::ProcessQueue(std::string* file) :
        procQueue()
    
    // body
    {
        std::ifstream inputFile; 
        std::string curLine;                   // store current parsed line
        char cstr[256];                        // cstring to store current line
        int pid = 0;                            

        // parse input file - get int
        inputFile.open(*file);
        while (!inputFile.eof()) {

            getline(inputFile, curLine);

            if (curLine.length() != 0) {
                char *end = nullptr;
                strcpy(cstr, curLine.c_str());

                // 10 -> decimal
                int AT = (int) strtol(cstr, &end, 10);  // long int to int
                int TC = (int) strtol(end, &end, 10);
                int CB = (int) strtol(end, &end, 10);
                int IO = (int) strtol(end, NULL, 10);  

                // enqueue
                procQueue.push_back(anntsai::Process(pid++, AT, TC, CB, IO));

            }
        }
        inputFile.close();
    };
}