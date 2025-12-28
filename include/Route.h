#pragma once

#include <string>

namespace HttpUtils {

struct RouteSegment {
	enum class Type { Literal, Parameter };

	Type type;
	std::string value;
};

struct Route {
	std::vector<RouteSegment> segments;
};
}