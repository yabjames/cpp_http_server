#pragma once
#include "../include/HttpServer.h"
#include <string_view>

class HttpParser {
  public:
	static bool parse(std::string_view buffer, HttpServer::Request &out);

  private:
	static bool parse_method(std::string_view buffer, std::string_view &method,
							 size_t &offset);

	static bool parse_route(std::string_view buffer, std::string_view &route,
							size_t &offset);

	static bool parse_body(std::string_view buffer, std::string_view &body,
						   const size_t &offset);
};
