#include <iostream>
#include <map>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include "runner/FunctionParams.h"
#include "runner/StreamingSidecarRunner.h"

#define LISTENQUEUESIZE 10
#define PORT 8080
#define POLLTIMEOUT 5

static volatile sig_atomic_t terminate = 0;

void die(std::string msg) {
    std::cerr << msg << std::endl;
    exit(1);
}

void handle_signal(int signum) {
    if (signum == SIGKILL) {
        // Time to wrap up
        terminate = 1;
    }
}

int main() {
    std::cout << "Woof" << std::endl;
    std::map<std::string, std::string> map;
    FunctionParams param(map, std::cin, std::cout);
    StreamingSidecarRunner ssr;

    // Ignore dieing children threads
    signal(SIGCHLD, SIG_IGN);
    int sockfd, newsockfd, portno, pid;
    socklen_t clilen;
    struct sockaddr_in serv_addr, cli_addr;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        die("Could not open socket");
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = PORT;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        die("Error on binding");
    }
    listen(sockfd, LISTENQUEUESIZE);
    clilen = sizeof(cli_addr);
        struct pollfd status;
        bzero((pollfd *) &status, sizeof(status));
        status.fd = sockfd;
        status.events = POLLIN;
    while(true) {
        poll(&status, 1, POLLTIMEOUT);
        if (status.revents == 0) {
            // No waiting data
            // Check sigkill flag
            if (terminate) {
                break;
            }
            continue;
        }
        // Maybe switch to poll logic here
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        pid = fork();

        if (pid == 0) {
            // Child process
            close(sockfd); // Close parent listening socket
            write(newsockfd, "Hewwo.\n", 7);
            close(newsockfd);
            break;
        } else if (pid > 0) {
            // Parent process
            close(newsockfd); // Close connection thread socket
        } else {
            die("Failed to fork connection thread");
        }
    }
    close(sockfd);

    //ssr.run(param);
    return 0;
}
