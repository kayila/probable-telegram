#include <algorithm>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <poll.h>
#include <signal.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include "headertoobigexception.h"
#include "Line.h"
#include "runner/FunctionParams.h"
#include "runner/StreamingSidecarRunner.h"

#define LISTENQUEUESIZE 10
#define PORT 8080
#define POLLTIMEOUT 5
#define HEADERMAXSIZE 8192

static volatile sig_atomic_t terminate = 0;

void die(std::string msg) {
    std::cerr << msg << std::endl;
    exit(1);
}

void handle_kill(int signum) {
    if (signum == SIGINT) {
        // Time to wrap up
        terminate = 1;
    }
}

bool isWhitespace(const int c) {
    return std::isspace(c);
}

std::string trim(const std::string &s) {

    std::string::const_iterator start = std::find_if_not(s.begin(), s.end(), isWhitespace);
    std::string::const_iterator end   = std::find_if_not(s.rbegin(), s.rend(), isWhitespace).base();
    return (start<end ? std::string(start, end) : std::string());
}



/*
 * Find a line within a multiline character array. A line is defined as a
 * sequence of chars starting at pos and ending with \r\n
 *
 * buf - Character buffer to search in.
 * size - size of the character buffer.
 * pos - position to start the search.
 */
Line* findLine(char *buf, int size, int pos) {
    int start = pos;

    if (pos == 0)
        pos = 1;
    if (pos>=size) {
        return new Line(0);
    }
    for (int x=pos; x < size; x++) {
        if (buf[x] == '\n'
                && buf[x-1] == '\r') {
            Line* ret = new Line(x - start - 1);
            ret->start(start)->end(x)->line(buf+start)->found(true);
            return ret;
        } else if (buf[x] == 0) {
            return new Line(0);
        }
    }
    return new Line(0);
}


void readFullHeader(int sockfd, char* buffer, int bufsize) {
    bzero(buffer, bufsize);
    for (int x=0; x<bufsize; x++) {
        // Maybe poll here
        // TODO Handle socket close
        read(sockfd, buffer+x, 1);
        if (buffer[x] == '\n'
                && x > 3) {
            if (buffer[x-1] == '\r'
                    && buffer[x-2] == '\n'
                    && buffer[x-3] == '\r' ) {
                // \r\n\r\n
                return;
            }
        }
    }
    throw HeaderTooBigException();
}

FunctionParams parseParams(int sockfd) {
    std::map<std::string, std::string> envMap;

    // read header
    char buffer[HEADERMAXSIZE];
    readFullHeader(sockfd, buffer, HEADERMAXSIZE);

    // buffer now holds the entire header
    std::string fullHeader(buffer);


    // Read the first line, store. update position to end
    // loop this
    // read the next line, if it's a space, append it to the previousy one and update position to end
    // if it does not start with a space, drop it and don't update position

    Line* requestLine = findLine(buffer, HEADERMAXSIZE, 0);
    unsigned int pos = requestLine->end() + 1;
    while(true) {
        std::string fullLine("");
        Line* line = findLine(buffer, HEADERMAXSIZE, pos);
        if (!line->found())
            break;
        pos = line->end() + 1;
        fullLine.append(line->line(), line->length());

        // Read ahead for additional lines
        while(true) {
            line = findLine(buffer, HEADERMAXSIZE, line->end()+1);
            if (line->found() && line->length() > 0
                    && (line->line()[0] == ' '
                        || line->line()[0] == '\t')
                    ) {
                fullLine.append(line->line(), line->length());
                pos = line->end() + 1;
            } else {
                break;
            }
        }

        // TODO Compress whitespace

        int splitPos = fullLine.find_first_of(':');
        std::string headerName = trim(fullLine.substr(0,splitPos));
        if (headerName.size() == 0) {
            continue;
        }
        std::string headerValue = trim(fullLine.substr(splitPos+1, fullLine.size()));
        if (envMap.count(headerName) == 1) {
            envMap[headerName] = envMap[headerName] + ", " + headerValue;
        } else {
            envMap[headerName] = headerValue;
        }
        //printf("start: %d\nend: %d\n", linepos.start, linepos.end);
        // if (pos == -1)
        //     break;
        if (line->found())
            printf("==%.*s==\n", line->length(), line->line());
    };


    std::cout << requestLine->line() << std::endl;
    for (auto it = envMap.begin(); it != envMap.end(); it++) {
        std::cout << it->first << "=>" << it->second << std::endl;
    }
    //pos = fullHeader.find('\r\n', pos);

    /*
    FunctionParams fp;
    return fp;
    */

}
int main() {
    std::cout << "Woof" << std::endl;
    std::map<std::string, std::string> map;
    FunctionParams param(map, std::cin, std::cout);
    StreamingSidecarRunner ssr;

    // Ignore dieing children threads
    signal(SIGCHLD, SIG_IGN);
    signal(SIGINT, handle_kill);
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
            parseParams(newsockfd);
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

    int sock_opts = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sock_opts, sizeof(int));
    close(sockfd);

    //ssr.run(param);
    return 0;
}
