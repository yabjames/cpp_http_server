//
// Created by james on 12/26/25.
//

#pragma once

#include <optional>
#include <string_view>
#include <string>
#include <unordered_map>


class PathParams {
public:
	explicit PathParams(
		const std::unordered_map<std::string, std::string>& path_params)
		: path_params(path_params) {
	}

	PathParams() = default;

	std::optional<std::string_view> get_path_param(std::string_view key);

	void add_param(std::string_view key, std::string_view value);

private:
	std::unordered_map<std::string, std::string> path_params;

};