#ifndef DISPATCH_SERVER_HPP
#define DISPATCH_SERVER_HPP

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
