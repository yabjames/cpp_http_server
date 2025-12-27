#pragma once

#include "AtomicQueue.h"
#include "PathParams.h"
#include "Route.h"

#include <atomic>
#include <functional>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

class HttpServer {
  public:
	HttpServer();

	~HttpServer();

	std::atomic<bool> stop_flag{};

	struct Request {
		std::string_view method;
		std::string_view route;
		std::string_view body;
		PathParams path_params;
	};

	struct Response {
		std::string_view header;
		std::string body;
		int status;
	};

	using Handler = std::function<void(const Request &, Response &)>;

	static bool is_valid_request(std::string &request_buffer,
								 ssize_t bytes_read);

	void get_mapping(std::string_view route, const Handler &fn);

	void post_mapping(std::string_view route, const Handler &fn);

	void put_mapping(std::string_view route, const Handler &fn);

	void patch_mapping(std::string_view route, const Handler &fn);

	void delete_mapping(std::string_view route, const Handler &fn);

	void head_mapping(std::string_view route, const Handler &fn);

	void options_mapping(std::string_view route, const Handler &fn);

	void connect_mapping(std::string_view route, const Handler &fn);

	void trace_mapping(std::string_view route, const Handler &fn);

	/**
	 * Tells the server to start listening/accepting requests from a specified
	 * port. This function is blocking.
	 */
	void listen(int port);

	/**
	 * Initializes a thread to start listening/accepting requests from a
	 * specified port. This function is non-blocking, so only use one active
	 * `listen()` or `start_listening()` method call for any given time.
	 */
	void start_listening(int port);

	/**
	 * Tells the server to stop listening/accepting requests.
	 */
	void stop_listening();

  private:
	AtomicQueue<int> queue;

	std::vector<std::thread> threads;

	std::unordered_map<std::string_view,
					   std::vector<std::pair<HttpUtils::Route, Handler>>>
		routes;

	std::unordered_map<std::string_view, std::vector<std::string_view>>
		route_path_params;

	void store_conn_fd(int conn_fd);

	int listener_fd{-1};

	std::string_view get_method(int conn_fd, std::string_view path,
								size_t &method_itr, bool &continues);

	/*
	 * @brief return a listener socket file descriptor
	 */
	static int get_listener_socket(int port);

	/**
	 * @brief Should be passed into a thread() worker to send a response back to
	 * an HTTP client.
	 */
	void handle_client();
};
