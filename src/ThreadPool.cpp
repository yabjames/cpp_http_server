#include "../include/ThreadPool.h"
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>

ThreadPool::ThreadPool() {
    for (int i = 0; i < Constants::max_worker_count; i++) {
        threads.emplace_back(&ThreadPool::handle_client, this);
    }
}

void ThreadPool::store_conn_fd(int conn_fd) {
    queue.push(conn_fd);
 }

void ThreadPool::handle_client() {
    while (true) {
        // Read the incoming HTTP request
        char buffer[4096];
        int conn_fd {};

        // if queue is empty
        if (!queue.pop(conn_fd)) continue;

        ssize_t bytes_read = recv(conn_fd, buffer, sizeof(buffer) - 1, 0);

        if (bytes_read <= 0) {
            close(conn_fd);
            continue;
        }

        buffer[bytes_read] = '\0';  // Null-terminate for safety

        std::string msg = "{\"msg\": \"hello there\"}";
        const char *response=
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: 0\r\n"
        "Connection: close\r\n"
        "\r\n";

        int bytes_sent = send(conn_fd, response, strlen(response), 0);
        if (bytes_sent == -1) {
            std::cerr << "\n\n" << strerror(errno) << ": issue sending message to connection\n";
        }

        close(conn_fd);
    }
    std::cout << "thread ending\n";
}
