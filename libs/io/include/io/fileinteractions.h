#pragma once

#include "IO/ssldigest.h"

// #define BIT7Z_USE_NATIVE_STRING avoided since vcpkg build comes without it
// #define BIT7Z_AUTO_FORMAT moved to cmake target_compile_definitions
#include <bit7z/bitarchivereader.hpp>
#include <bit7z/bitextractor.hpp>

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

/*    
    bool unzipArchive(QString archive, QString targetPath);
    bool executeExternalWaiting(QString executablePath, QString working_directory = "");
    QString calculateHashFromFile(QString sFile);
    QString readFullFileString(QString filename);
    QString readFirstFileLine(QString filename);
    int writeFileString(QString filename, QString filecontent);
    int removeFile(QString pathstring, QString filename);
    void copyFolderTo(QString folderPath, QString targetPath);
    void copyFileTo(QString filePath, QString targetPath);
    void removeFolder(QString folderPath);
    void createFolder(QString folderPath);
*/

namespace Updater2::IO {
	bool cleanUpRemainingTempFiles();

    bool unzipArchive(const std::filesystem::path& inArchive, const std::filesystem::path& outDir);
    //bool unzipArchive(const std::string& inArchive, const std::string& outDir);

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