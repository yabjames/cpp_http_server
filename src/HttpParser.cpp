//
// Created by james on 12/17/25.
//

#include "HttpParser.h"

#include <HttpServer.h>
#include <iostream>
#include <string_view>

bool HttpParser::parse(const std::string_view buffer,
					   HttpServer::Request &out) {
	size_t offset{0};
	if (parse_method(buffer, out.method, offset) &&
		parse_route(buffer, out.route, offset) &&
		parse_body(buffer, out.body, offset)) {
		return true;
	}
	return false;
}

bool HttpParser::parse_method(const std::string_view buffer,
							  std::string_view &method, size_t &offset) {
	offset = buffer.find(' ');
	if (offset == std::string_view::npos) {
		std::cerr << "Invalid request formatting: no spaces\n";
		return false;
	}
	method = buffer.substr(0, offset);
	offset++;
	return true;
}

bool HttpParser::parse_route(const std::string_view buffer,
							 std::string_view &route, size_t &offset) {
	const size_t route_start_itr = offset;
	offset = buffer.find(' ', route_start_itr);
	if (offset == std::string_view::npos) {
		std::cerr << "Invalid request formatting: no valid route\n";
		return false;
	}
	route = buffer.substr(route_start_itr, offset - route_start_itr);
	offset++;
	return true;
}

bool HttpParser::parse_body(std::string_view buffer, std::string_view &body,
							const size_t &offset) {
	size_t body_start_itr = buffer.find("\r\n\r\n", offset);
	if (body_start_itr == std::string_view::npos) {
		std::cerr << "Invalid request formatting: the start of the request "
					 "body is malformed\n";
		return false;
	}
	body_start_itr += 4;
	body = buffer.substr(body_start_itr, buffer.size() - body_start_itr);
	return true;
}
