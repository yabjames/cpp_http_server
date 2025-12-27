//
// Created by james on 12/26/25.
//

#pragma once

#include <optional>
#include <string_view>
#include <unordered_map>


class PathParams {
public:
	std::optional<std::string_view> get_path_param(std::string_view key);

	void add_param(std::string_view key, std::string_view value);

private:
	std::unordered_map<std::string_view, std::string_view> path_params;

};