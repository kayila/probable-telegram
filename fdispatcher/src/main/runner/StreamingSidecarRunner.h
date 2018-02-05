#ifndef STREAMINGSIDECARRUNNER_H
#define STREAMINGSIDECARRUNNER_H

#include "SidecarRunner.h"

class StreamingSidecarRunner: public SidecarRunner {
    public:
        int runFunc(FunctionParams params);
};

#endif /* STREAMINGSIDECARRUNNER_H */
