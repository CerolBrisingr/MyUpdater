#pragma once

#include "io/ssldigest.h"

// #define BIT7Z_USE_NATIVE_STRING avoided since vcpkg build comes without it
// #define BIT7Z_AUTO_FORMAT moved to cmake target_compile_definitions
#include <bit7z/bitarchivereader.hpp>

#include <filesystem>
#include <fstream>
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

	std::string calculateMd5HashFromFile(const std::filesystem::path& filename);
	bool compareMd5Hashes(std::string_view hash1, std::string_view hash2);
    
	bool isFolder(const std::filesystem::path& path_in);
	bool isFile(const std::filesystem::path& path_in);
	void writeStringAsFile(const std::filesystem::path& filename, std::string_view filecontent);
	std::string readFirstLineInFile(const std::filesystem::path& filename);
	std::string readTextFile(const std::filesystem::path& filename);

    bool copyFileTo(const std::filesystem::path& filePath, const std::filesystem::path& targetPath, bool isClean = true);
    bool copyFileTo(const std::filesystem::path& filePath, const std::filesystem::path& targetPath, std::error_code& ec, bool isClean = true);
    bool copyFolderInto(const std::filesystem::path& folderPath, const std::filesystem::path& targetPath, bool isClean = true);
    bool copyFolderInto(const std::filesystem::path& folderPath, const std::filesystem::path& targetPath, std::error_code& ec, bool isClean = true);

    bool createFolder(const std::filesystem::path& folderPath, bool isClean = true) noexcept;
    bool createFolder(const std::filesystem::path& folderPath, std::error_code& ec, bool isClean = true) noexcept;
    void removeFile(const std::filesystem::path& filename);
    bool removeFileNoThrow(const std::filesystem::path& filename, bool isClean = true) noexcept;
    bool removeFileNoThrow(const std::filesystem::path& filename, std::error_code& ec, bool isClean = true) noexcept;
    std::uintmax_t removeFolderRecursively(const std::filesystem::path& filename);
    std::uintmax_t removeFolderRecursively(const std::filesystem::path& filename, std::error_code& ec);

} // namespace Updater2::IO