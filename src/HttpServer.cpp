#include "../include/HttpServer.h"
#include "../include/constants.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>

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

HttpServer::HttpServer() {
    for (int i = 0; i < Constants::max_worker_count; i++) {
        threads.emplace_back(&HttpServer::handle_client, this);
    }
}

void HttpServer::listen(int port) {
    int listener_file_descriptor = get_listener_socket(port);
    if (listener_file_descriptor < 0) {
        std::cerr << "unable to obtain listener socket, exiting\n";
        std::exit(EXIT_FAILURE);
    }
    while (1) {
        struct sockaddr_storage incoming_addr {};
        socklen_t addr_size {sizeof(incoming_addr)};

        int conn_file_descriptor = accept(listener_file_descriptor, (struct sockaddr*)&incoming_addr, &addr_size);
        if (conn_file_descriptor == -1) {
            std::cerr << strerror(errno) << ": issue trying to accept incoming connection\n";
            break;
        }
        this->store_conn_fd(conn_file_descriptor);
    }

    close(listener_file_descriptor);
}


void HttpServer::store_conn_fd(int conn_fd) {
    queue.push(conn_fd);
 }

void HttpServer::handle_client() {
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
        size_t method_itr = request.find(' ');
        if (method_itr == std::string_view::npos) {
            close(conn_fd);
            std::cerr << "Invalid request formatting: no spaces\n";
            continue;
        }

        // check for valid method
        std::string_view method = request.substr(0, method_itr);
        std::cout << "method: " << method << '\n';

        // get the route which is the second word
        size_t route_start = method_itr + 1;
        size_t route_end = request.find(' ', route_start);
        if (route_end == std::string_view::npos) {
            close(conn_fd);
            std::cerr << "Invalid request formatting: no valid route\n";
            continue;
        }

        std::string_view route = request.substr(route_start, route_end - route_start);
        std::cout << "route: " << route << '\n';

        // get body
        size_t req_body_start = request.find("\r\n\r\n") + 4;
        if (req_body_start == std::string_view::npos) {
            close (conn_fd);
            std::cerr << "Invalid request formatting: the start of the request body is malformed\n";
        }

        std::string_view req_body = request.substr(req_body_start, request.size() - req_body_start);
        std::cout << "body: " << req_body << '\n';

        // TODO: create a map that has a key route and function pointer

        Response res {};
        switch (method_hash(method)) {
            case compile_time_method_hash("GET"): {
                const Request req { request, ""};
                Handler route_fn = routes[method][route];
                route_fn(req, res);
                break;
            }
            case compile_time_method_hash("POST"):
            case compile_time_method_hash("PUT"):
            case compile_time_method_hash("PATCH"):
            case compile_time_method_hash("DELETE"):
            case compile_time_method_hash("HEAD"):
            case compile_time_method_hash("OPTIONS"):
            case compile_time_method_hash("CONNECT"):
            case compile_time_method_hash("TRACE"): {
                const Request req { request, std::string(req_body)};
                Handler route_fn = routes[method][route];
                route_fn(req, res);
                break;
            }
            default: {
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
                continue;
            }
        }

        const std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Length: " + std::to_string(res.body.size()) + "\r\n"
        "Connection: close\r\n"
        "\r\n" +
        std::string(res.body);

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

int HttpServer::get_listener_socket(int port) {
    std::string port_str = std::to_string(port);
    struct addrinfo hints {};
    struct addrinfo* addrinfo_ptr {};
    struct addrinfo* results {};
    int socket_file_descriptor {};

    hints.ai_family = AF_UNSPEC;     // can be IPv4 or 6
    hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
    hints.ai_flags = AI_PASSIVE;     // fill in IP for us

    int status = getaddrinfo(Constants::hostname, port_str.c_str(), &hints, &results);
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

    int listen_status = ::listen(socket_file_descriptor, Constants::backlog);
    if (listen_status == -1) {
        std::cerr << "\n\n" << strerror(errno) << ": issue trying to call listen()\n";
        return 1;
    }

    return socket_file_descriptor;
}

void HttpServer::get_mapping(std::string_view route, const Handler& fn) {
    routes["GET"][route] = fn;
}

void HttpServer::post_mapping(std::string_view route, const Handler& fn) {
    routes["POST"][route] = fn;
}

void HttpServer::put_mapping(std::string_view route, const Handler& fn) {
    routes["PUT"][route] = fn;
}

void HttpServer::patch_mapping(std::string_view route, const Handler& fn) {
    routes["PATCH"][route] = fn;
}

void HttpServer::delete_mapping(std::string_view route, const Handler& fn) {
    routes["DELETE"][route] = fn;
}

void HttpServer::head_mapping(std::string_view route, const Handler& fn) {
    routes["HEAD"][route] = fn;
}

void HttpServer::options_mapping(std::string_view route, const Handler& fn) {
    routes["OPTIONS"][route] = fn;
}

void HttpServer::connect_mapping(std::string_view route, const Handler& fn) {
    routes["CONNECT"][route] = fn;
}

void HttpServer::trace_mapping(std::string_view route, const Handler& fn) {
    routes["TRACE"][route] = fn;
}
