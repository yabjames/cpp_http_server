#pragma once

#include <array>
#include <thread>

namespace HttpServer {
    class ServerThreadPool {
    private:
        std::array<bool, 8> thread_pool_occupied;


    public:
        ServerThreadPool();

        /**
         * @brief Should be passed into a thread() worker to send a response back to an HTTP client.
         *        A side-effect is that it will toggle the occupancy in the thread_pool_occupied member array
         * @param thread_pool_id Tells the SendResponse function which array index to toggle in the thread_pool_occupied array
         * @param conn_file_descriptor Used to send the response through the associated socket
         */
        void send_response(int thread_pool_id, int conn_file_descriptor);

        /**
         * @brief check and return the next unoccupied thread idx
         */
        int occupy();


    };
}
