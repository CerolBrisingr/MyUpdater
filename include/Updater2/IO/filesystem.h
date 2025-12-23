#pragma once

#include <filesystem>
#include <iostream>
#include <string_view>
#include <string>
#include <fstream>

namespace Updater2::IO {

	void printCurrentPath();
	bool isFolder(std::string_view path_in);
	bool writeStringAsFile(const std::string &filename, std::string_view filecontent);
	std::string readFirstLineInFile(const std::string& filename);


} // namespace Updater2::IO