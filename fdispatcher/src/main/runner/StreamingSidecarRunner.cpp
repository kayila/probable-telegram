#include "StreamingSidecarRunner.h"
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <iostream>
#include <vector>

std::vector<char*> split(const std::string &str, const char *delimiter) {
    std::vector<char*> output;
    char *input = new char[str.length()+1];
    std::strcpy(input, str.c_str());
    char *token = std::strtok(input, delimiter);
    while (token != NULL) {
        output.push_back(token);
        token = std::strtok(NULL, delimiter);
    }
    return output;
}

int StreamingSidecarRunner::runFunc(FunctionParams params) {
    // Declare the pipes for the child process to control
    // var[0] = read end of the pipe
    // var[1] = write end of the pipe
    int childin[2];
    int childout[2];
    int childerr[2];

    // Create the pipes
    pipe(childin);
    pipe(childout);
    pipe(childerr);

    // Connect FunctionParams to pipes

    pid_t pid = fork();

    if(pid == 0) {
        // Store the command to run
        const char* cmd = std::getenv("FCOMMAND");
        std::cout << "Going to run command: " << cmd << std::endl;

        // Change stdin/out/err FD to the pipes above
        dup2(childin[1], STDIN_FILENO);
        dup2(childout[0], STDOUT_FILENO);
        dup2(childerr[0], STDERR_FILENO);

        // Close FD not needed by the child
        for (int x=0; x<2; x++) {
            close(childin[x]);
            close(childout[x]);
            close(childerr[x]);
        }

        // TODO Set up env
        //const char* envp[5];

        // Parse the command string into array of null terminated strings
        const char* delim = " ";
        std::vector<char*> cmdvec = split(cmd, delim);
        cmdvec.push_back(NULL); // Required for the execvpe below

        char **cmdarr = cmdvec.data();

        execvp(cmdarr[0], cmdarr); // , envp);
    } else if (pid > 0) {
        // Close unneeded FD for the parent
        close(childin[0]);
        close(childout[1]);
        close(childerr[1]);
    } else {
        std::cout << "fork() failed!" << std::endl;
    }

    return 0;
}
