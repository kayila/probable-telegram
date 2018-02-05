#ifndef RUNNER_H
#define RUNNER_H

#include "FunctionParams.h"

class Runner {
    public:
        virtual int run(FunctionParams params) = 0;
};

#endif
