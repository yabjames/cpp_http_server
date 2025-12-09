#pragma once

#include <functional>
#include <thread>
#include <vector>
#include <string>
#include <atomic>
#include <string_view>
#include "../include/AtomicQueue.h"

class HttpServer {
public:
    HttpServer();

    ~HttpServer();

    std::atomic<bool> stop_flag;

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

    /**
     * Tells the server to start listening/accepting requests from a specified port. This function is blocking.
     */
    void listen(int port);

    /**
     * Initializes a thread to start listening/accepting requests from a specified port. This function is non-blocking,
     * so only use one active `listen()` or `start_listening()` method call for any given time.
     */
    void start_listening(int port);

    /**
     * Tells the server to stop listening/accepting requests.
     */
    void stop_listening();

private:
    AtomicQueue<int> queue;

    std::vector<std::thread> threads;

    std::unordered_map<std::string_view, std::unordered_map<std::string_view, Handler>> routes;

    void store_conn_fd(int conn_fd);

    int listener_fd {-1};

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
