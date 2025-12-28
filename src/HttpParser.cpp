//
// Created by james on 12/17/25.
//

#include "HttpParser.h"

#include <HttpServer.h>
#include <iostream>
#include <string_view>

HttpParser::Route HttpParser::compile_route(const std::string_view path) {
	Route route {};
	for (std::string_view segment : split_path(path)) {
		if (segment.size() > 2 && segment.front() == '{' && segment.back() == '}') {
			RouteSegment route_segment = {
				RouteSegment::Type::Parameter,
				std::string(segment.substr(1, segment.size() - 2))
			};
			route.segments.push_back(route_segment);
		}
		RouteSegment route_segment = {
			RouteSegment::Type::Literal,
			std::string(segment)
		};
		route.segments.push_back(route_segment);
	}

	return route;
}

/**
 *
 * @param route
 * @param request request.path_params will be modified by match_route
 * @return
 */
bool HttpParser::match_route(const HttpUtils::Route& route, HttpServer::Request& request) {
	const std::vector<std::string_view> request_segments = split_path(request.route);

	// Segment count must match
	if (request_segments.size() != route.segments.size()) {
		return false;
	}

	// Compare segment by segment
	for (size_t i = 0; i < route.segments.size(); ++i) {
		const HttpUtils::RouteSegment route_seg = route.segments[i];
		const std::string_view req_seg = request_segments[i];

		if (route_seg.type == HttpUtils::RouteSegment::Type::Literal) {
			// Literal must match exactly
			if (route_seg.value != req_seg) {
				return false;
			}
		}
	}
	for (size_t i = 0; i < route.segments.size(); ++i) {
		if (route.segments[i].type == HttpUtils::RouteSegment::Type::Literal) continue;

		const HttpUtils::RouteSegment route_seg = route.segments[i];
		const std::string_view req_seg = request_segments[i];
		request.path_params.add_param(route_seg.value, req_seg);
	}

	return true;
}

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

std::vector<std::string_view> HttpParser::split_path(std::string_view path) {
    std::vector<std::string_view> segments;
    size_t start = 0;

	// If there is only the root '/' for the client request
	if (path == "/") {
		segments.push_back(path);
		return segments;
	}

    while (start < path.size()) {
        if (path[start] == '/') {
            ++start;
            continue;
        }

        size_t end = path.find('/', start);

        // found the last segment
        if (end == std::string_view::npos) {
            segments.push_back(path.substr(start));
            break;
        }

        segments.push_back(path.substr(start, end - start));
        start = end;
    }

    return segments;
}
