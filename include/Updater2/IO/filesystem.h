#pragma once

#include <filesystem>
#include <iostream>
#include <string_view>

namespace Updater2::IO {

	void printCurrentPath();
	bool isFolder(std::string_view path_in);

} // namespace Updater2::IO