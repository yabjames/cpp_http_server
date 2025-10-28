#include "../include/ServerThreadPool.h"
#include <chrono>
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <thread>
#include <unistd.h>
using namespace HttpServer;

ServerThreadPool::ServerThreadPool() {
    this->thread_pool_occupied.fill(false);
}

void ServerThreadPool::send_response(int thread_pool_id, int conn_file_descriptor) {
    const char* msg = "{\"msg\": \"hello there\"}";
    std::string response =
    std::string("HTTP/1.1 200 OK\n") +
    "Content-Length: " + std::to_string(std::strlen(msg)) + "\n"
    "Content-Type: application/json\n\n" +
    msg;

    int response_len = response.size();
    int bytes_sent = send(conn_file_descriptor, response.c_str(), response_len, 0);
    if (bytes_sent == -1) {
        std::cerr << "\n\n" << strerror(errno) << ": issue sending message to connection\n";
    }

    this->thread_pool_occupied.at(thread_pool_id) = false;
    close(conn_file_descriptor);
}

int ServerThreadPool::occupy() {
    int idx = 0;
    int size = this->thread_pool_occupied.size();
    while (1) {
        for (idx = 0; idx < size; idx++) {
            if (!this->thread_pool_occupied[idx]) {
                this->thread_pool_occupied[idx] = true;
                return idx;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    return idx;
}
