#ifndef FUNCTIONPARAMS_H
#define FUNCTIONPARAMS_H

#include <map>
#include <iostream>

class FunctionParams {
    public:
        FunctionParams(std::map<string, string>, std::istream, std::ostream);
        std::map<string, string> getEnv();
        std::istream getInput();
        std::ostream getOutput();
    private:
        std::map<string, string> env;
        std::istream input;
        std::ostream output;
}

#endif /* FUNCTIONPARAMS_H */
