#include "../include/ServerThreadPool.h"
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
using namespace HttpServer;

ServerThreadPool::ServerThreadPool() {
}

void ServerThreadPool::send_response(int conn_file_descriptor) {
    const char* msg = "{\"msg\": \"hello there\"}";
    std::string response =
    std::string("HTTP/1.1 200 OK\r\n") +
    "Content-Length: " + std::to_string(std::strlen(msg)) + "\r\n"
    "Content-Type: application/json\r\n"
    "Connection: close\r\n"
    "\r\n" +
    std::string(msg);

    int response_len = response.size();
    int bytes_sent = send(conn_file_descriptor, response.c_str(), response_len, 0);
    if (bytes_sent == -1) {
        std::cerr << "\n\n" << strerror(errno) << ": issue sending message to connection\n";
    }

    close(conn_file_descriptor);
}
