#pragma once
#include <string>
#include <string_view>
#include <vector>

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

