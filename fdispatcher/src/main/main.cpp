#include <iostream>
#include <map>
#include <string>
#include "runner/FunctionParams.h"
#include "runner/StreamingSidecarRunner.h"

int main() {
    std::cout << "Woof" << std::endl;
    std::map<std::string, std::string> map;
    FunctionParams param(map, std::cin, std::cout);
    StreamingSidecarRunner ssr;//();

    ssr.run(param);
    return 0;
}
