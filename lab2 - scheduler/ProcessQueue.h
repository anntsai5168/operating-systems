# ifndef PROCESSQUEUE_H
# define PROCESSQUEUE_H

# include "Process.h"
# include <deque>
# include <fstream>
# include <string>

namespace anntsai {

    class ProcessQueue {
        public:
            std::deque<Process> procQueue;

            ProcessQueue(std::string *file);
    };
}

# endif