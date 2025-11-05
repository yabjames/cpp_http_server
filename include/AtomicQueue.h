#include <mutex>
#include <condition_variable>
#include <queue>

template<typename T>
class AtomicQueue {
private:
    std::mutex mutex;
    std::condition_variable cond_var;
    std::queue<T> queue;

public:
    AtomicQueue() = default;
    ~AtomicQueue() = default;

    // Push an item and notify one waiting consumer.
    void push(const T& value) {
        {
            // lock ends when going out of scope
            std::lock_guard<std::mutex> lock(mutex);
            queue.push(value);
        }
        cond_var.notify_one();
    }

    bool pop(T& result) {
        std::unique_lock<std::mutex> lock(mutex);

        // when notified and if queue not empty, then proceed
        cond_var.wait(lock, [this]{ return !queue.empty(); });
        result = queue.front();
        queue.pop();

        // unlock out of scope
        return true;
    }

};
