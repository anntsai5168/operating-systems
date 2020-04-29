/* read input */

#ifndef RANDOM_H
#define RANDOM_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>


class Random {

    public:
        Random(std::string *rfile);
        ~Random();

        std::string *rfile;

        int ttl_cnt;      // ttl random nums
        int cur_cnt = 0;  // cur count 
        int rnum;         // random number

        int getRandom(int FT_size);

};


#endif
