#pragma once

#include <array>
#include <functional>
#include <thread>
#include <semaphore>
#include <vector>
#include "constants.h"
#include "../include/AtomicQueue.h"

class HttpServer {
public:
    HttpServer();

    enum Method {
        GET,
        POST,
        PUT,
        PATCH,
        DELETE,
        HEAD,
        OPTIONS,
        CONNECT,
        TRACE
    };

    struct Request {
        std::string route;
        std::string body;
        Method method;
    };

    struct Response {
        std::string header;
        std::string body;
        int status;
    };

    using Handler = std::function<void(const Request&, Response&)>;

    void get_mapping(std::string route, const Handler& fn);

    void store_conn_fd(int conn_fd);

    void listen(std::string port);

private:
    AtomicQueue<int> queue;
    std::vector<std::thread> threads;
    std::unordered_map<std::string, Handler> get_routes;

    /**
        * @brief Should be passed into a thread() worker to send a response back to an HTTP client.
        *        A side-effect is that it will toggle the occupancy in the thread_pool_occupied member array
        * @param thread_pool_id Tells the SendResponse function which array index to toggle in the thread_pool_occupied array
        * @param conn_file_descriptor Used to send the response through the associated socket
        */
    void handle_client();


};
