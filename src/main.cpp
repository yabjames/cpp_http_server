#include <arpa/inet.h>
#include <cstdio>
#include <iostream>
// #include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


int main(int argc, char *argv[]) {
    struct addrinfo hints {};
    struct addrinfo* results {};
    struct addrinfo* p {};
    char ipstr[INET6_ADDRSTRLEN];

    hints.ai_family = AF_UNSPEC;     // can be IPv4 or 6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in IP for us

    const char* hostname = "www.duckduckgo.com";
    int status = getaddrinfo(hostname, nullptr, &hints, &results);
    if (status != 0) {
        std::cout << stderr << "gai error: " << gai_strerror(status) << '\n';
        return 1;
    }

    std::cout << "IP addresses for " << hostname << ": \n\n";

    for (p = results; p != nullptr; p = p->ai_next) {
        void* addr {};
        const char* ipver;
        struct sockaddr_in* ipv4;
        struct sockaddr_in6* ipv6;

        if (p->ai_family == AF_INET) { // IPv4
            ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        } else { // IPv6
            ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = "IPv6";
        }

        inet_ntop(p->ai_family, addr, ipstr, sizeof ipstr);
        std::cout << " " << ipver << ": " << ipstr << '\n';
    }

    // std::cout << "address length: " << server_info->ai_addrlen << '\n';

    freeaddrinfo(results);
}
