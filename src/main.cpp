#include <arpa/inet.h>
#include <asm-generic/socket.h>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/resource.h>
#include "../include/constants.h"
#include "../include/ThreadPool.h"

/*
 * @brief return a listener socket file descriptor
 */
int get_listener_socket() {
    struct addrinfo hints {};
    struct addrinfo* addrinfo_ptr {};
    struct addrinfo* results {};
    int socket_file_descriptor {};

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
        socket_file_descriptor = socket(addrinfo_ptr->ai_family, addrinfo_ptr->ai_socktype, addrinfo_ptr->ai_protocol);
        if (socket_file_descriptor == -1) {
            std::cerr << "\n\n" << strerror(errno) << ": issue fetching the socket file descriptor\n";
            continue;
        }

        // set socket options
        int yes = 1;
        int sockopt_status = setsockopt(socket_file_descriptor, SOL_SOCKET,SO_REUSEADDR, &yes, sizeof(int));
        if (sockopt_status == -1) {
            std::cerr << "\n\n" << strerror(errno) << ": issue setting socket options\n";
            return 1;
        }

        // associate the socket descriptor with the port passed into getaddrinfo()
        int bind_status = bind(socket_file_descriptor, addrinfo_ptr->ai_addr, addrinfo_ptr->ai_addrlen);
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

    int listen_status = listen(socket_file_descriptor, Constants::backlog);
    if (listen_status == -1) {
        std::cerr << "\n\n" << strerror(errno) << ": issue trying to call listen()\n";
        return 1;
    }

    return socket_file_descriptor;
}

int main(int argc, char *argv[]) {
    int listener_file_descriptor = get_listener_socket();
    if (listener_file_descriptor < 0) {
        std::cerr << "unable to obtain listener socket, exiting\n";
        return 1;
    }
    ThreadPool thread_pool {};

    while (1) {
        struct sockaddr_storage incoming_addr {};
        socklen_t addr_size {sizeof(incoming_addr)};

        int conn_file_descriptor = accept(listener_file_descriptor, (struct sockaddr*)&incoming_addr, &addr_size);
        if (conn_file_descriptor == -1) {
            std::cerr << strerror(errno) << ": issue trying to accept incoming connection\n";
            break;
        }
        thread_pool.store_conn_fd(conn_file_descriptor);
    }

    close(listener_file_descriptor);
}
