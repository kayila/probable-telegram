#include "FunctionParams.h"

FunctionParams::FunctionParams(std::map<std::string, std::string> &envmap, std::istream &instream, std::ostream &outstream) {
    env = envmap;
    input = &instream;
    output = &outstream;
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
