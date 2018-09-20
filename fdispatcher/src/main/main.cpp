#include <algorithm>
#include <cstring>
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

// Checks a char buffer for two newlines
// b = buffer, x = position, o = offset
#define NLNL(b, x, o) (b[x-o] == '\n' && b[x-o-1] == '\n')
// Checks a char buffer for CRNL
// b = buffer, x = position, o = offset
#define NLCR(b, x, o) (b[x-o] == '\n' && b[x-o-1] == '\r')

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
Line& findLine(char *buf, int size, int pos) {
    int start = pos;

    if (pos == 0)
        pos = 1;
    if (pos>=size) {
        return *new Line(0);
    }
    for (int x=pos; x < size; x++) {
        if (buf[x] == '\n'
            ) {
                unsigned int length = x-start;
                length -= (x > 0 && buf[x-1] == '\r') ? 1 : 0;
            Line& ret = *new Line(length);
            ret.start(start).end(x).line(buf+start).found(true);
            return ret;
        } else if (buf[x] == 0) {
            return *new Line(0);
        }
    }
    return *new Line(0);
}


void readFullHeader(int sockfd, char* buffer, int bufsize) {
    std::memset(buffer, 0, bufsize);
    for (int x=0; x<bufsize; x++) {
        // Maybe poll here
        // TODO Handle socket close
        read(sockfd, buffer+x, 1);
        if ( (x>0 && NLNL(buffer, x, 0))
                || (x>2 && NLCR(buffer, x, 0) && NLCR(buffer, x, 2))) {
            return;
        }
    }
    throw HeaderTooBigException();
}

FunctionParams& parseParams(int sockfd) {
    auto& envMap = *new std::map<std::string, std::string>();
    //std::map<std::string, std::string>

    // read header
    char buffer[HEADERMAXSIZE];
    readFullHeader(sockfd, buffer, HEADERMAXSIZE);

    // buffer now holds the entire header
    std::string fullHeader(buffer);


    // Read the first line, store. update position to end
    // loop this
    // read the next line, if it's a space, append it to the previousy one and update position to end
    // if it does not start with a space, drop it and don't update position

    Line& requestLine = findLine(buffer, HEADERMAXSIZE, 0);
    unsigned int pos = requestLine.end() + 1;
    while(true) {
        std::string fullLine("");
        Line& line = findLine(buffer, HEADERMAXSIZE, pos);
        if (!line.found())
            break;
        pos = line.end() + 1;
        fullLine.append(line.line(), line.length());

        // Read ahead for additional lines
        while(true) {
            int end=line.end();
            delete(&line);
            line = findLine(buffer, HEADERMAXSIZE, end+1);
            if (line.found() && line.length() > 0
                    && (line.line()[0] == ' '
                        || line.line()[0] == '\t')
                    ) {
                fullLine.append(line.line(), line.length());
                pos = line.end() + 1;
            } else {
                break;
            }
        }
        // Memory leak?
        //delete(&line);

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
        if (fullLine.size() > 0)
            printf("==%s==\n", fullLine.c_str());
    };


    std::cout << requestLine.line() << std::endl;
    for (auto it = envMap.begin(); it != envMap.end(); it++) {
        std::cout << it->first << "=>" << it->second << std::endl;
    }
    FunctionParams& fp = *new FunctionParams(sockfd, envMap);
    return fp;
}

int main() {
    std::cout << "Woof" << std::endl;
    std::map<std::string, std::string> map;
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
    std::memset((char *) &serv_addr, 0, sizeof(serv_addr));
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
            FunctionParams& fp = parseParams(newsockfd);
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
