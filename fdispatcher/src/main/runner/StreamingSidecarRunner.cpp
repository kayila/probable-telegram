#include "StreamingSidecarRunner.h"
#include <iostream>

int StreamingSidecarRunner::run(FunctionParams params) {
    return runFunc(params);
}

int StreamingSidecarRunner::runFunc(FunctionParams params) {
    std::cout << "hi!" << std::endl;
    return 0;
}
