#pragma once

#include "IO/ssldigest.h"

#include <filesystem>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <cstddef>
#include <memory>
#include <system_error>
#include <iterator>

namespace Updater2::IO {
	bool cleanUpRemainingTempFiles();

	std::string calculateMd5HashFromFile(const std::string& filename);
	bool compareMd5Hashes(std::string_view hash1, std::string_view hash2);
    
	void printCurrentPath();
	bool isFolder(std::string_view path_in);
	bool isFile(std::string_view path_in);
	void writeStringAsFile(std::string_view filename, std::string_view filecontent);
	std::string readFirstLineInFile(const std::string& filename);
	std::string readTextFile(const std::string& filename);

    void removeFile(std::string_view filename);
    bool removeFileNoThrow(std::string_view filename, bool isClean = true) noexcept;
    bool removeFileNoThrow(std::string_view filename, std::error_code& ec, bool isClean = true) noexcept;


} // namespace Updater2::IO