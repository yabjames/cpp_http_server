#pragma once

#include "HttpServer.h"

#include <string_view>
#include <vector>

class HttpParser {
  public:
	struct RouteSegment {
		enum class Type { Literal, Parameter };
		Type type;
		std::string value;
	};

	struct Route {
		std::vector<RouteSegment> segments;
	};

	struct PathParam {
		std::string_view name;
		std::string_view value;
	};

	static bool match_route(
		const Route& route,
		std::string_view request_path,
		std::vector<PathParam>& out_params
	);

	static Route compile_route(std::string_view path);

	static bool parse(std::string_view buffer, HttpServer::Request &out);

	static std::vector<std::string_view> split_path(std::string_view path);

  private:
	static bool parse_method(std::string_view buffer, std::string_view &method,
							 size_t &offset);

	static bool parse_route(std::string_view buffer, std::string_view &route,
							size_t &offset);

	static bool parse_body(std::string_view buffer, std::string_view &body,
						   const size_t &offset);

};
