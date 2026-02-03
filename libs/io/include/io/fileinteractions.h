#pragma once

#include "io/ssldigest.h"

// #define BIT7Z_USE_NATIVE_STRING avoided since vcpkg build comes without it
// #define BIT7Z_AUTO_FORMAT moved to cmake target_compile_definitions
#include <bit7z/bitarchivereader.hpp>

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
#include <stdint.h>

namespace Updater2::IO::Archive {
    struct Information {
        uint32_t itemsCount{ 0 };
        uint32_t foldersCount{ 0 };
        uint32_t filesCount{ 0 };
        uint64_t packSize{ 0 };
        uint64_t size{ 0 };
    };
} // namespace Updater2::IO::Archive

namespace Updater2::IO {

    bool createProcess(const std::filesystem::path& processPath, const std::wstring& commandLineArgs);

	bool cleanUpRemainingTempFiles();

    bool unzipArchive(const std::filesystem::path& inArchive, const std::filesystem::path& outDir);
    auto inspectArchive(const std::filesystem::path& archivePath) -> std::optional<Archive::Information>;

	std::string calculateMd5HashFromFile(const std::string& filename);
	bool compareMd5Hashes(std::string_view hash1, std::string_view hash2);
    
	void printCurrentPath();
	bool isFolder(std::string_view path_in);
	bool isFile(std::string_view path_in);
	void writeStringAsFile(std::string_view filename, std::string_view filecontent);
	std::string readFirstLineInFile(const std::string& filename);
	std::string readTextFile(const std::string& filename);

    bool copyFileTo(std::string_view filePath, std::string_view targetPath, bool isClean = true);
    bool copyFileTo(std::string_view filePath, std::string_view targetPath, std::error_code& ec, bool isClean = true);
    bool copyFolderInto(std::string_view folderPath, std::string_view targetPath, bool isClean = true);
    bool copyFolderInto(std::string_view folderPath, std::string_view targetPath, std::error_code& ec, bool isClean = true);

    bool createFolder(std::string_view folderPath, bool isClean = true) noexcept;
    bool createFolder(std::string_view folderPath, std::error_code& ec, bool isClean = true) noexcept;
    void removeFile(std::string_view filename);
    bool removeFileNoThrow(std::string_view filename, bool isClean = true) noexcept;
    bool removeFileNoThrow(std::string_view filename, std::error_code& ec, bool isClean = true) noexcept;
    std::uintmax_t removeFolderRecursively(std::string_view filename);
    std::uintmax_t removeFolderRecursively(std::string_view filename, std::error_code& ec);

} // namespace Updater2::IO