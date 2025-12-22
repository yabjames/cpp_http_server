#pragma once

#include <atomic>
#include <condition_variable>
#include <mutex>
#include <queue>

template <typename T> class AtomicQueue {
  private:
	std::mutex mutex;
	std::condition_variable cond_var;
	std::queue<T> queue;

  public:
	AtomicQueue() = default;
	~AtomicQueue() = default;

	// Push an item and notify one waiting consumer.
	void push(const T &value) {
		{
			// lock ends when going out of scope
			std::lock_guard<std::mutex> lock(mutex);
			queue.push(value);
		}
		cond_var.notify_one();
	}

	bool pop(T &result, const std::atomic<bool> &stop_flag) {
		std::unique_lock<std::mutex> lock(mutex);

		// when notified and if queue not empty, then proceed
		// if the stop_flag is true, then it will instantly return from the
		// pop()
		while (queue.empty()) {
			if (stop_flag.load()) {
				return false;
			}
			cond_var.wait_for(lock, std::chrono::milliseconds(100));
		}

		result = queue.front();
		queue.pop();

		// unlock out of scope
		return true;
	}
};
