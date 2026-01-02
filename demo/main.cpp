#include <iostream>

#include "HttpServer.h"

int main() {
	HttpServer server{};

	server.get_mapping(
		"/test", [](HttpServer::Request &req, HttpServer::Response &res) {
			res.body = "testing new api route";
		});

	server.post_mapping(
		"/test2/{id}/foo/{user}",
		[](HttpServer::Request &req, HttpServer::Response &res) {
			std::stringstream ss;
			try {
				ss << "{\"id\":" << "\"" << req.path_params.get_path_param("id").value() << "\","
				<< "\"user\":" << "\"" << req.path_params.get_path_param("user").value() << "\""
				<< "}";
				res.body = ss.str();
			} catch (const std::bad_optional_access &e) {
				res.body = "could not get path parameter";
			}
		});

	try {
		server.listen(3490);
	} catch (const std::exception &err) {
		std::cerr << err.what() << '\n';
		return EXIT_FAILURE;
	}
}