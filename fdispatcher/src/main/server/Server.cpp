#include <iostream>
#include <csignal>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

// TODO: Do we care enough to not hard-code these?
const uint16_t port = 9090;
const int backlog_size = 5;

int server_fd = -1;

void print_error(const char *file, int line) {
    int buflen = 256;
    char msg[256];

    strerror_r(errno, (char*)&msg, buflen);
    fprintf(stderr, "Error: %s at %s:%i.\r\n", (char*)&msg, file, line);
}

void cleanup() {
    // Don't call close() unless the socket was actually opened.
    if (server_fd != -1) {
        close(server_fd);
    }
}

bool handle_request(int client_fd) {
    char buffer[256];
    ssize_t bytes_read;
    ssize_t bytes_written;

    memset(buffer, 0, 256);
    bytes_read = read(client_fd, buffer, 255);

    if (bytes_read < 0) {
        print_error(__FILE__, __LINE__);
        return false;
    }

    std::cout << "Message: " << buffer << std::endl;
    bytes_written = write(client_fd, "I got your message", 18);
    if (bytes_written < 0) {
        print_error(__FILE__, __LINE__);
        return false;
    }

    close(client_fd);

    return true;
}

int main(int argc, char *argv[]) {
    int client_fd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_len;

    struct sockaddr_in server_addr = {
        .sin_family = AF_INET,
        .sin_port   = htons(port),
        .sin_addr   = (struct in_addr){
            .s_addr = INADDR_ANY
        },
    };

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        print_error(__FILE__, __LINE__);
    }

    if (bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        print_error(__FILE__, __LINE__);
        return errno;
    }

    if (listen(server_fd, backlog_size) != 0) {
        print_error(__FILE__, __LINE__);
        return errno;
    }

    while (true) {
        client_addr_len = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_addr_len);
        if (client_fd < 0) {
            print_error(__FILE__, __LINE__);
            return errno;
        }

        handle_request(client_fd);
    }

    return 0;
}
