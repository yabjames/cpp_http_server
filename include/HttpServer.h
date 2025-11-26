#pragma once

#include <functional>
#include <thread>
#include <vector>
#include <string>
#include <string_view>
#include "../include/AtomicQueue.h"

class HttpServer {
public:
    HttpServer();

    struct Request {
        std::string_view route;
        std::string body;
    };

    struct Response {
        std::string_view header;
        std::string body;
        int status;
    };

    using Handler = std::function<void(const Request&, Response&)>;

    void get_mapping(std::string_view route, const Handler& fn);

    void post_mapping(std::string_view route, const Handler& fn);

    void put_mapping(std::string_view route, const Handler& fn);

    void patch_mapping(std::string_view route, const Handler& fn);

    void delete_mapping(std::string_view route, const Handler& fn);

    void head_mapping(std::string_view route, const Handler& fn);

    void options_mapping(std::string_view route, const Handler& fn);

    void connect_mapping(std::string_view route, const Handler& fn);

    void trace_mapping(std::string_view route, const Handler& fn);

    void listen(int port);

private:
    AtomicQueue<int> queue;

    std::vector<std::thread> threads;

    std::unordered_map<std::string_view, std::unordered_map<std::string_view, Handler>> routes;

    void store_conn_fd(int conn_fd);

    /*
     * @brief return a listener socket file descriptor
     */
    int get_listener_socket(int port);

    /**
        * @brief Should be passed into a thread() worker to send a response back to an HTTP client.
        *        A side-effect is that it will toggle the occupancy in the thread_pool_occupied member array
        * @param thread_pool_id Tells the SendResponse function which array index to toggle in the thread_pool_occupied array
        * @param conn_file_descriptor Used to send the response through the associated socket
        */
    void handle_client();

};
