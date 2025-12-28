#include "HttpServer.h"
#include "constants.h"

#include <HttpParser.h>
#include <cstring>
#include <iostream>
#include <netdb.h>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <unordered_map>

constexpr size_t compile_time_method_hash(const std::string_view method) {
	size_t hash = 0;
	for (const char c : method) {
		hash += c;
	}
	return hash;
}

size_t method_hash(const std::string_view method) {
	size_t hash = 0;
	for (const char c : method) {
		hash += c;
	}
	return hash;
}

HttpServer::HttpServer() {
	stop_flag.store(false);
	for (int i = 0; i < Constants::max_worker_count; i++) {
		threads.emplace_back(&HttpServer::handle_client, this);
	}
}

HttpServer::~HttpServer() {
	this->stop_listening();
	for (auto &thread : threads) {
		thread.join();
	}
	threads.clear();
}

void HttpServer::listen(int port) {
	listener_fd = get_listener_socket(port);
	if (listener_fd < 0) {
		throw std::runtime_error("unable to obtain listener socket, exiting\n");
	}

	std::cout << "server listening now...\n";
	while (!stop_flag.load()) {
		struct sockaddr_storage incoming_addr{};
		socklen_t addr_size{sizeof(incoming_addr)};

		const int conn_file_descriptor = accept(
			listener_fd, reinterpret_cast<struct sockaddr *>(&incoming_addr),
			&addr_size);
		if (conn_file_descriptor == -1) {
			// If we're stopping, accept failures are expected; don't spam logs.
			if (stop_flag.load())
				break;

			// Interrupted system call - retry.
			if (errno == EINTR)
				continue;

			// If socket was closed or not valid, stop the loop quietly.
			if (errno == EBADF || errno == EINVAL || errno == EOPNOTSUPP)
				break;

			// Otherwise log and continue or break as appropriate.
			throw std::runtime_error("unable to obtain a valid connection file "
				"descriptor, exiting\n");
		}
		this->store_conn_fd(conn_file_descriptor);
	}

	if (listener_fd != -1) {
		close(listener_fd);
		listener_fd = -1;
	}
}

void HttpServer::start_listening(int port) {
	threads.emplace_back(&HttpServer::listen, this, port);
}

void HttpServer::stop_listening() {
	stop_flag.store(true);

	if (listener_fd != -1) {
		shutdown(listener_fd, SHUT_RDWR); // interrupt the accept()
		close(listener_fd);
		listener_fd = -1;
	}
}

void HttpServer::store_conn_fd(int conn_fd) { queue.push(conn_fd); }

bool HttpServer::is_valid_request(std::string &request_buffer,
                                  ssize_t bytes_read) {
	// Check if the request is empty
	if (bytes_read <= 0) {
		std::cerr << "Invalid request formatting: 0 bytes read\n";
		return false;
	}
	request_buffer[bytes_read] = '\0'; // Null-terminate for safety
	return true;
}

void HttpServer::handle_client() {
	while (!stop_flag.load()) {
		// Read the incoming HTTP request
		std::string request_buffer;
		request_buffer.resize(4096);

		int conn_fd{};

		// if queue is empty
		if (!queue.pop(conn_fd, stop_flag)) {
			if (stop_flag.load())
				return;
			continue;
		}

		ssize_t bytes_read =
			recv(conn_fd, request_buffer.data(), request_buffer.size(), 0);
		if (!is_valid_request(request_buffer, bytes_read)) {
			close(conn_fd);
			continue;
		}

		Request req{};
		if (!HttpParser::parse(request_buffer, req)) {
			close(conn_fd);
			continue;
		}

		Response res{};
		std::string response{};
		switch (method_hash(req.method)) {
		case compile_time_method_hash("GET"):
		case compile_time_method_hash("DELETE"):
		case compile_time_method_hash("HEAD"):
		case compile_time_method_hash("OPTIONS"):
		case compile_time_method_hash("CONNECT"):
		case compile_time_method_hash("TRACE"): {
			req.body = "";
			for (const auto &[route, route_fn] : routes[req.method]) {
				if (HttpParser::match_route(route, req)) {
					// when the route is matched, fn should run and allow fn definitions to use pathParams
					res.status = 200;
					res.status_name = "OK";
					route_fn(req, res);
					break;
				}
			}
			if (res.status == 200)
				break;
			res.status = 404;
			res.status_name = "Not Found";
			res.body = R"({"error": "The requested endpoint does not exist"})";
		}
		case compile_time_method_hash("POST"):
		case compile_time_method_hash("PUT"):
		case compile_time_method_hash("PATCH"): {
			for (const auto &[route, route_fn] : routes[req.method]) {
				if (HttpParser::match_route(route, req)) {
					// when the route is matched, fn should run and allow fn definitions to use pathParams
					res.status = 200;
					res.status_name = "OK";
					route_fn(req, res);
					break;
				}
			}
			if (res.status == 200)
				break;
			res.status = 404;
			res.status_name = "Not Found";
			res.body = R"({"error": "The requested endpoint does not exist"})";
			break;
		}
		default: {
			res.status = 500;
			res.status_name = "Error";
			res.body =
				R"({\"error\": \"The request does not have a valid HTTP method\"})";
		}
		}
		const ssize_t bytes_sent =
			send(conn_fd, res.str().c_str(), res.str().size(), 0);
		if (bytes_sent == -1) {
			close(conn_fd);
			std::cerr << "\n\n"
				<< strerror(errno)
				<< ": issue sending message to connection\n";
			continue;
		}
		close(conn_fd);
	}
}

int HttpServer::get_listener_socket(const int port) {
	const std::string port_str = std::to_string(port);
	addrinfo hints{};
	const addrinfo *addrinfo_ptr{};
	addrinfo *results{};
	int socket_file_descriptor{};

	hints.ai_family = AF_UNSPEC; // can be IPv4 or 6
	hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
	hints.ai_flags = AI_PASSIVE; // fill in IP for us

	int status =
		getaddrinfo(Constants::hostname, port_str.c_str(), &hints, &results);
	if (status != 0) {
		throw std::runtime_error("gai error: " +
		                         std::string(gai_strerror(status)));
	}

	// find the first file descriptor that does not fail
	for (addrinfo_ptr = results; addrinfo_ptr != nullptr;
	     addrinfo_ptr = addrinfo_ptr->ai_next) {
		socket_file_descriptor =
			socket(addrinfo_ptr->ai_family, addrinfo_ptr->ai_socktype,
			       addrinfo_ptr->ai_protocol);
		if (socket_file_descriptor == -1) {
			std::cerr << "\n\n"
				<< strerror(errno)
				<< ": issue fetching the socket file descriptor\n";
			continue;
		}

		// set socket options
		int yes = 1;
		int sockopt_status = setsockopt(socket_file_descriptor, SOL_SOCKET,
		                                SO_REUSEADDR, &yes, sizeof(int));
		if (sockopt_status == -1) {
			throw std::runtime_error(std::string(strerror(errno)) +
			                         ": issue setting socket options");
		}

		// associate the socket descriptor with the port passed into
		// getaddrinfo()
		int bind_status = bind(socket_file_descriptor, addrinfo_ptr->ai_addr,
		                       addrinfo_ptr->ai_addrlen);
		if (bind_status == -1) {
			std::cerr << "\n\n"
				<< strerror(errno)
				<< ": issue binding the socket descriptor with a port";
			continue;
		}

		break;
	}

	freeaddrinfo(results);

	if (addrinfo_ptr == nullptr) {
		throw std::runtime_error(std::string(strerror(errno)) +
		                         ": failed to bind port to socket");
	}

	int listen_status = ::listen(socket_file_descriptor, Constants::backlog);
	if (listen_status == -1) {
		throw std::runtime_error(std::string(strerror(errno)) +
		                         ": issue trying to call listen()");
	}

	return socket_file_descriptor;
}

void HttpServer::get_mapping(const std::string_view route, const Handler &fn) {
	routes["GET"].emplace_back(HttpParser::path_to_route(route), fn);
}

void HttpServer::post_mapping(const std::string_view route, const Handler &fn) {
	routes["POST"].emplace_back(HttpParser::path_to_route(route), fn);
}

void HttpServer::put_mapping(const std::string_view route, const Handler &fn) {
	routes["PUT"].emplace_back(HttpParser::path_to_route(route), fn);
}

void HttpServer::patch_mapping(const std::string_view route,
                               const Handler &fn) {
	routes["PATCH"].emplace_back(HttpParser::path_to_route(route), fn);
}

void HttpServer::delete_mapping(const std::string_view route,
                                const Handler &fn) {
	routes["DELETE"].emplace_back(HttpParser::path_to_route(route), fn);
}

void HttpServer::head_mapping(const std::string_view route, const Handler &fn) {
	routes["HEAD"].emplace_back(HttpParser::path_to_route(route), fn);
}

void HttpServer::options_mapping(const std::string_view route,
                                 const Handler &fn) {
	routes["OPTIONS"].emplace_back(HttpParser::path_to_route(route), fn);
}

void HttpServer::connect_mapping(const std::string_view route,
                                 const Handler &fn) {
	routes["CONNECT"].emplace_back(HttpParser::path_to_route(route), fn);
}

void HttpServer::trace_mapping(const std::string_view route,
                               const Handler &fn) {
	routes["TRACE"].emplace_back(HttpParser::path_to_route(route), fn);
}