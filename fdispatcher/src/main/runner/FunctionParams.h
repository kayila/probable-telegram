#ifndef FUNCTIONPARAMS_H
#define FUNCTIONPARAMS_H

#include <map>
#include <iostream>
#include <string>

class FunctionParams {
    public:
        FunctionParams(std::map<std::string, std::string>&, std::istream&, std::ostream&);
        std::map<std::string, std::string> getEnv();
        std::istream *getInput();
        std::ostream *getOutput();
    private:
        std::map<std::string, std::string> env;
        std::istream *input;
        std::ostream *output;
};

#endif /* FUNCTIONPARAMS_H */
