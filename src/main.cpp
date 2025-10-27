#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include "../include/constants.h"


int main(int argc, char *argv[]) {
    struct addrinfo hints {};
    struct addrinfo* results {};
    struct addrinfo* addrinfo_ptr {};
    char ipstr[INET6_ADDRSTRLEN] {};

    hints.ai_family = AF_UNSPEC;     // can be IPv4 or 6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in IP for us

    int status = getaddrinfo(Constants::hostname, nullptr, &hints, &results);
    if (status != 0) {
        std::cerr << stderr << "gai error: " << gai_strerror(status) << '\n';
        return 1;
    }

    std::cout << "IP addresses for " << Constants::hostname << ": \n\n";

    for (addrinfo_ptr = results; addrinfo_ptr != nullptr; addrinfo_ptr = addrinfo_ptr->ai_next) {
        void* address = nullptr;
        const char* ipversion = nullptr;
        struct sockaddr_in* ipv4 = nullptr;
        struct sockaddr_in6* ipv6 = nullptr;

        if (addrinfo_ptr->ai_family == AF_INET) { // IPv4
            ipv4 = (struct sockaddr_in *)addrinfo_ptr->ai_addr;
            address = &(ipv4->sin_addr);
            ipversion = "IPv4";
        } else { // AF_INET6 (IPv6)
            ipv6 = (struct sockaddr_in6 *)addrinfo_ptr->ai_addr;
            address = &(ipv6->sin6_addr);
            ipversion = "IPv6";
        }

        inet_ntop(addrinfo_ptr->ai_family, address, ipstr, sizeof ipstr);
        std::cout << " " << ipversion << ": " << ipstr << '\n';
    }

    // getting the socket file descriptor
    int socket_file_descriptor {};
    socket_file_descriptor = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if (socket_file_descriptor == -1) {
        std::cerr << "\n\nerror " << strerror(errno) << ": issue fetching the socket file descriptor\n";
        return 1;
    }
    std::cout << "socket file descriptor: " << socket_file_descriptor << '\n';

    // associate the socket descriptor with the port passed into getaddrinfo()
    int bind_status = bind(socket_file_descriptor, results->ai_addr, results->ai_addrlen);
    if (bind_status == -1) {
        std::cerr << "\n\nerror " << strerror(errno) << ": issue binding the socket descriptor with a port\n";
        return 1;
    }

    int listen_status = listen(socket_file_descriptor, Constants::backlog);
    if (listen_status == -1) {
        std::cerr << "\n\nerror " << strerror(errno) << ": issue trying to call listen()\n";
        return 1;
    }

    struct sockaddr_storage incoming_addr {};
    socklen_t addr_size {sizeof(incoming_addr)};
    int conn_file_descriptor = accept(socket_file_descriptor, (struct sockaddr*)&incoming_addr, &addr_size);
    if (conn_file_descriptor == -1) {
        std::cerr << "\n\nerror " << strerror(errno) << ": issue trying to accept incoming connection\n";
        return 1;
    }

    freeaddrinfo(results);
}
