//
// Created by james on 12/26/25.
//

#include "PathParams.h"

std::optional<std::string_view> PathParams::get_path_param(
	const std::string_view key) {
	if (path_params.find(key) != path_params.end()) {
		return path_params[key];
	}
	return std::nullopt;
}

void PathParams::add_param(std::string_view key,
                           std::string_view value) {
	path_params[key] = value;
}