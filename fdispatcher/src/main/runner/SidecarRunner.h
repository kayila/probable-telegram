#ifndef SIDECARRUNNER_H
#define SIDECARRUNNER_H

#include "Runner.h"

class SidecarRunner {
    public:
        int run(FunctionParams params);
        virtual int runFunc(FunctionParams params) = 0;
};

#endif /* SIDECARRUNNER_H */
