#include <iostream>

#include "../include/HttpServer.h"

int main() {
	HttpServer server{};

	server.get_mapping(
		"/test", [](const HttpServer::Request &, HttpServer::Response &res) {
			res.body = "testing new api route";
		});

	server.get_mapping(
		"/test2", [](const HttpServer::Request &, HttpServer::Response &res) {
			res.body = "this is the other route";
		});

	try {
		server.listen(3490);
	} catch (const std::exception &err) {
		std::cerr << err.what() << '\n';
		return EXIT_FAILURE;
	}
}
