#ifndef STREAMINGSIDECARRUNNER_H
#define STREAMINGSIDECARRUNNER_H

#include "SidecarRunner.h"

class StreamingSidecarRunner: public SidecarRunner {
    public:
        int run(FunctionParams params); // TODO: use SidecarRunner::run()
        int runFunc(FunctionParams params);
};

#endif /* STREAMINGSIDECARRUNNER_H */
