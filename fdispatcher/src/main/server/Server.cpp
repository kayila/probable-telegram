#include <iostream>
#include <csignal>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <signal.h>
#include "Server.hpp"

static bool dispatchServerInitialized = false;
static bool dispatchServerRunning = true;

void DispatchServer::SignalHandler(int sig) {
    switch (sig) {
    case SIGINT:
        dispatchServerRunning = false;
        break;
    }
}

void DispatchServer::_global_setup() {
    // If we've already done the global initialization, bail immediately.
    if (dispatchServerInitialized == true) {
        return;
    }

    struct sigaction signal_action = {{SignalHandler}};

    dispatchServerInitialized = true;

    sigaction(SIGINT, &signal_action, NULL);
}

void DispatchServer::printError(const char *file, int line) {
    const int buffer_size = 256; /* ARBITRARY: 256 byte buffer. */

#if defined(__GLIBC__) && defined(__USE_GNU)
    // HACK: The C++ standard library used by g++ and clang++ relies on
    // GNU extensions. As such, it automatically defines _GNU_SOURCE.
    // And undefining it breaks the standard library.
    // Thus, I have to do this garbage.
    char *msg = NULL;
    msg = strerror_r(errno, msg, buffer_size);
#else
    char msg[buffer_size];
    strerror_r(errno, msg, buffer_size);
#endif

    fprintf(stderr, "error: %s at %s:%i (errno %i).\r\n", msg, file, line, errno);
}

bool DispatchServer::handleRequest(int clientFd) {
    const int buffer_size = 4096; /* ARBITRARY: 4KB buffer. */
    char buffer[buffer_size];
    ssize_t bytes_read;
    ssize_t bytes_written;

    memset(buffer, 0, buffer_size);
    bytes_read = read(clientFd, buffer, buffer_size - 1);

    if (bytes_read < 0) {
        printError(__FILE__, __LINE__);
        return false;
    }

    std::cout << "Message: " << buffer << std::endl;
    bytes_written = write(clientFd, "I got your message", 18);
    if (bytes_written < 0) {
        printError(__FILE__, __LINE__);
        return false;
    }

    close(clientFd);

    return true;
}

int DispatchServer::createServer(int port) {
    int serverFd;

    struct in_addr server_inaddr = { /*.s_addr =*/ INADDR_ANY };
    struct sockaddr_in server_addr = {
        /*.sin_family =*/ AF_INET,
        /*.sin_port   =*/ htons(port),
        /*.sin_addr   =*/ server_inaddr
    };

    serverFd = socket(AF_INET, SOCK_STREAM, 0);
    if (serverFd < 0) {
        printError(__FILE__, __LINE__);
        exit(errno);
    }

    if (bind(serverFd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        printError(__FILE__, __LINE__);
        exit(errno);
    }

    if (listen(serverFd, /* backlog size = */ 5) != 0) {
        printError(__FILE__, __LINE__);
        exit(errno);
    }

    return serverFd;
}

void DispatchServer::mainLoop() {
    int clientFd;
    struct sockaddr_in clientAddress;
    socklen_t clientAddressLength;

    while (dispatchServerRunning) {
        clientAddressLength = sizeof(clientAddress);
        clientFd = accept(serverFd, (struct sockaddr*)&clientAddress, &clientAddressLength);

        if (errno == EINTR) {
            break;
        }

        if (clientFd < 0) {
            printError(__FILE__, __LINE__);
            exit(errno);
        }

        handleRequest(clientFd);
    }
}

void DispatchServer::stop() {
    std::cout << "Terminating server." << std::endl;
    if (close(serverFd) != 0) {
        printError(__FILE__, __LINE__);
        exit(1);
    }
}

DispatchServer::DispatchServer(int port) {
    _global_setup();
    serverFd = createServer(port);
}

DispatchServer::~DispatchServer() {
    stop();
}

int main(int argc, char *argv[]) {
    DispatchServer server = DispatchServer(9090);
    server.mainLoop();

    return 0;
}
