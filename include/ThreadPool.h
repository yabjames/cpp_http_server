#pragma once

#include <array>
#include <thread>
#include <semaphore>
#include <vector>
#include "constants.h"
#include "../include/AtomicQueue.h"

class ThreadPool {
private:
    AtomicQueue<int> queue;
    std::vector<std::thread> threads;


public:
    ThreadPool();

    void store_conn_fd(int conn_fd);

    /**
        * @brief Should be passed into a thread() worker to send a response back to an HTTP client.
        *        A side-effect is that it will toggle the occupancy in the thread_pool_occupied member array
        * @param thread_pool_id Tells the SendResponse function which array index to toggle in the thread_pool_occupied array
        * @param conn_file_descriptor Used to send the response through the associated socket
        */
    void handle_client();

};
