#ifndef FUNCTIONPARAMS_H
#define FUNCTIONPARAMS_H

#include <map>
#include <iostream>
#include <string>

class FunctionParams {
    public:
        FunctionParams(int& sockfd, std::map<std::string, std::string>&);
        ~FunctionParams();
        std::map<std::string, std::string> getEnv();
        std::istream *getInput();
        std::ostream *getOutput();
    private:
        int sockfd;
        std::map<std::string, std::string> env;
        std::istream *input;
        std::ostream *output;
};

#endif /* FUNCTIONPARAMS_H */
