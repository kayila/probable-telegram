#ifndef DISPATCH_SERVER_HPP
#define DISPATCH_SERVER_HPP

#if defined(__GLIBC__) && defined(__USE_GNU)
// HACK: The C++ standard library used by g++ and clang++ relies on
// GNU extensions. As such, it automatically defines _GNU_SOURCE.
// And undefining it breaks the standard library.
// HOWEVER, they provide a standards-compliant one under a different name.
// Thus, I have to do this garbage.
#error "DispatcxhServer::printError() needs a POSIX_compliant strerror_r(). Try building it on Alpine."
#endif

class DispatchServer {
public:
    DispatchServer(int port);
    ~DispatchServer();
    void mainLoop();
    void stop();
private:
    void printError(const char *file, int line);
    bool handleRequest(int client_fd);
    int createServer(int port);

    int serverFd;

    // Handle graceful termination.
    static void SignalHandler(int sig);
    static void _global_setup();
};


#endif /* DISPATCH_SERVER_HPP */
