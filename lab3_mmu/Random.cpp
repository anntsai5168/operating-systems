#include "Random.h"


std::ifstream inputFile;

// init
Random::Random(std::string *rfile)

{   // get ttl cnt
    std::string cur_line;
    inputFile.open(*rfile);

    getline(inputFile, cur_line);     
    std::istringstream stream(cur_line);
    stream >> ttl_cnt;
}
Random::~Random() { 
    inputFile.close();
}

    
int Random::getRandom(int FT_size) {

    std::string cur_line;

    if (cur_cnt == ttl_cnt) {                // wrap around
        cur_cnt = 0;
        inputFile.clear();
        inputFile.seekg(0, std::ios::beg);
        inputFile.ignore(1000, '\n');        //  ignore first line
    }

    getline(inputFile, cur_line);
    cur_cnt ++;
    std::istringstream stream(cur_line);
    stream >> rnum;

    return rnum % FT_size;
}
