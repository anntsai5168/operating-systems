# include "Process.h"

namespace anntsai {
    // initialize
    Process::Process(int pid, int AT, int TC, int CB, int IO):
        pid(pid),
        AT(AT),
        TC(TC),
        CB(CB),
        IO(IO),

        PRIO(0),
        FT(0),
        TT(0),
        IT(0),
        CW(0),

        state(STATE_CREATED), // Initially when a process arrives at the system it is put into CREATED state
        PREPRIOpreempted(false),
        dPRIO(0),
        rTC(TC),
        ranCB(0),
        ranIO(0),
        state_ts(AT)
    {};
}