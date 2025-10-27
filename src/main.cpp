#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "../include/constants.h"


int main(int argc, char *argv[]) {
    struct addrinfo hints {};
    struct addrinfo* results {};
    struct addrinfo* addrinfo_ptr {};
    int socket_file_descriptor {};
    // char ipstr[INET6_ADDRSTRLEN] {};

    hints.ai_family = AF_UNSPEC;     // can be IPv4 or 6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in IP for us

    int status = getaddrinfo(Constants::hostname, Constants::port, &hints, &results);
    if (status != 0) {
        std::cerr << stderr << " gai error: " << gai_strerror(status) << '\n';
        return 1;
    }

    // find the first file descriptor that does not fail
    for (addrinfo_ptr = results; addrinfo_ptr != nullptr; addrinfo_ptr = addrinfo_ptr->ai_next) {
        socket_file_descriptor = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
        if (socket_file_descriptor == -1) {
            std::cerr << "\n\n" << strerror(errno) << ": issue fetching the socket file descriptor\n";
            continue;
        }
        std::cout << "socket file descriptor: " << socket_file_descriptor << '\n';

        // set socket options
        int yes = 1;
        int sockopt_status = setsockopt(socket_file_descriptor, SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(int));
        if (sockopt_status == -1) {
            std::cerr << "\n\n" << strerror(errno) << ": issue setting socket options\n";
            return 1;
        }

        // associate the socket descriptor with the port passed into getaddrinfo()
        int bind_status = bind(socket_file_descriptor, results->ai_addr, results->ai_addrlen);
        if (bind_status == -1) {
            std::cerr << "\n\n" << strerror(errno) << ": issue binding the socket descriptor with a port\n";
            continue;
        }

        break;
    }

    freeaddrinfo(results);

    if (addrinfo_ptr == nullptr) {
        std::cerr << "\n\n" << strerror(errno) << ": failed to bind port to socket\n";
        return 1;
    }



    while (1) {
        int listen_status = listen(socket_file_descriptor, Constants::backlog);
        if (listen_status == -1) {
            std::cerr << "\n\n" << strerror(errno) << ": issue trying to call listen()\n";
            return 1;
        }

        struct sockaddr_storage incoming_addr {};
        socklen_t addr_size {sizeof(incoming_addr)};
        int conn_file_descriptor = accept(socket_file_descriptor, (struct sockaddr*)&incoming_addr, &addr_size);

        if (conn_file_descriptor == -1) {
            std::cerr << "\n\n" << strerror(errno) << ": issue trying to accept incoming connection\n";
            return 1;
        }

        const char* msg = "hello there good sir!";
        std::string response =
        std::string("HTTP/1.1 200 OK\n") +
        "Content-Length: " + std::to_string(strlen(msg)) + "\n"
        "Content-Type: raw\n\n" +
        msg;

        int response_len = response.size();
        std::cout << "response length: " << response_len << '\n';
        int bytes_sent = send(conn_file_descriptor, response.c_str(), response_len, 0);
        if (bytes_sent == -1) {
            std::cerr << "\n\n" << strerror(errno) << ": issue sending message to connection\n";
        }
        close(conn_file_descriptor);
    }

    close(socket_file_descriptor);
}
