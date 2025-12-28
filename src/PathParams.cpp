//
// Created by james on 12/26/25.
//

#include "PathParams.h"

#include <string>
#include <string_view>

std::optional<std::string_view> PathParams::get_path_param(
	const std::string_view key) {
	std::string skey{key};
	if (path_params.find(std::string{key}) != path_params.end()) {
		return path_params[skey];
	}
	return std::nullopt;
}

void PathParams::add_param(std::string_view key,
                           std::string_view value) {
	path_params[std::string{key}] = value;
}