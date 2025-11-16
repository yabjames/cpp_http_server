#include "../include/ThreadPool.h"
#include <cstring>
#include <iostream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <utility>

constexpr size_t compile_time_method_hash(std::string_view method) {
    size_t hash = 0;
    for (char c : method) {
        hash += c;
    }
    return hash;
}

size_t method_hash(std::string_view method) {
    size_t hash = 0;
    for (char c : method) {
        hash += c;
    }
    return hash;
}

ThreadPool::ThreadPool() {
    for (int i = 0; i < Constants::max_worker_count; i++) {
        threads.emplace_back(&ThreadPool::handle_client, this);
    }
}

void ThreadPool::listen(std::string port) {}

void ThreadPool::get_mapping(std::string route, const Handler& fn) {
    get_routes[route] = fn;
}

void ThreadPool::store_conn_fd(int conn_fd) {
    queue.push(conn_fd);
 }

void ThreadPool::handle_client() {
    while (true) {
        // Read the incoming HTTP request
        char request_buffer[4096];
        int conn_fd {};

        // if queue is empty
        if (!queue.pop(conn_fd)) continue;

        ssize_t bytes_read = recv(conn_fd, request_buffer, sizeof(request_buffer) - 1, 0);

        if (bytes_read <= 0) {
            close(conn_fd);
            std::cerr << "Invalid request formatting: 0 bytes read\n";
            continue;
        }
        request_buffer[bytes_read] = '\0';  // Null-terminate for safety

        std::string_view request {request_buffer};

        // find the method
        size_t methodItr = request.find(' ');
        if (methodItr == std::string_view::npos) {
            close(conn_fd);
            std::cerr << "Invalid request formatting: no spaces\n";
            continue;
        }

        // check for valid method
        std::string_view method = request.substr(0, methodItr);
        std::cout << "method: " << method << '\n';

        // get the route which is the second word
        size_t routeStart = methodItr + 1;
        size_t routeEnd = request.find(' ', routeStart);
        if (routeEnd == std::string_view::npos) {
            close(conn_fd);
            std::cerr << "Invalid request formatting: no valid route\n";
            continue;
        }

        std::string_view route = request.substr(routeStart, routeEnd - routeStart);
        std::cout << "route: " << route << '\n';

        // get body

        // TODO: create a map that has a key route and function pointer

        Response res {};
        switch (method_hash(method)) {
            case compile_time_method_hash("GET"): {
                std::cout << "SLDKFJLSDKSFJ";
                // check get_mappings
                for (std::pair<std::string, Handler> route_pair : get_routes) {
                    const Request req = { std::string(request), "", Method::GET};
                    // run the mapping if the route is correct
                    if (route == route_pair.first) {
                        route_pair.second(req, res);
                    }
                }
            }
                break;
            case compile_time_method_hash("POST"):
            case compile_time_method_hash("PUT"):
            case compile_time_method_hash("PATCH"):
            case compile_time_method_hash("DELETE"):
            case compile_time_method_hash("HEAD"):
            case compile_time_method_hash("OPTIONS"):
            case compile_time_method_hash("CONNECT"):
            case compile_time_method_hash("TRACE"):
            default:
                // TODO: throw an exeption here
                std::cout <<"default method\n";
                const char *response=
                "HTTP/1.1 200 OK\r\n"
                "Content-Length: 0\r\n"
                "Connection: close\r\n"
                "\r\n";

                int bytes_sent = send(conn_fd, response, strlen(response), 0);
                if (bytes_sent == -1) {
                    close(conn_fd);
                    std::cerr << "\n\n" << strerror(errno) << ": issue sending message to connection\n";
                    continue;
                }
                std::cout << request_buffer << "\n";

                close(conn_fd);

        }

        const std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: " + std::to_string(res.body.size()) + "\r\n"
        "Connection: close\r\n"
        "\r\n" +
        res.body;

        int bytes_sent = send(conn_fd, response.c_str(), response.size(), 0);
        if (bytes_sent == -1) {
            close(conn_fd);
            std::cerr << "\n\n" << strerror(errno) << ": issue sending message to connection\n";
            continue;
        }
        std::cout << request_buffer << "\n";
        close(conn_fd);

    }
    std::cout << "thread ending\n";
}
