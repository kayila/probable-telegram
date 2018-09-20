#include "FunctionParams.h"

FunctionParams::FunctionParams(int& sockfd, std::map<std::string, std::string> &envmap) {
    env = envmap;
    /*
    input = &instream;
    output = &outstream;
    */
}

FunctionParams::~FunctionParams() {
    delete(&env);
}

std::map<std::string, std::string> FunctionParams::getEnv() {
    return env;
}

std::istream *FunctionParams::getInput() {
    return input;
}

std::ostream *FunctionParams::getOutput() {
    return output;
}
