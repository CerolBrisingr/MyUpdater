#include "IO/fileinteractions.h"

namespace ssl = Updater2::SSL;
namespace fs = std::filesystem;

namespace Updater2::IO {

	namespace {
		constexpr std::string_view g_tempTextLocation{ "temp.txt" };
		constexpr std::size_t g_bufferSize{ 1024 * 1024 };  // 1MB

		struct TempFileJunkCollector {
			const fs::path path{ "" };
			explicit TempFileJunkCollector(const fs::path& p)
				:path{ p }
			{}

			TempFileJunkCollector(TempFileJunkCollector& rhs) = delete;
			TempFileJunkCollector& operator= (TempFileJunkCollector& rhs) = delete;

			~TempFileJunkCollector() {
				std::error_code ec;
				fs::remove(path, ec);
			}
		};

		bool verifyClean(bool isClean, const std::error_code& ec) {
			// Return true if isClean and no error code is set
			return isClean && !ec;
		}

		void ensureParentFolderExists(const fs::path& filename) {
			if (filename.has_parent_path()) {
				std::error_code ec;
				fs::create_directories(filename.parent_path(), ec);
				if (ec) {
					throw std::runtime_error("Failed to ensure necessary folder structure: " + ec.message());
				}
			}
		}
	} // namespace

	bool unzipArchive(const fs::path& inArchive, const fs::path& outDir)
	{
		// Looks like bit7z intends to focus on UTF-8 and also vcpkg compiles without BIT7Z_USE_NATIVE_STRING
		// File paths will therefore use .string()
		try { // bit7z classes can throw BitException objects
			bit7z::Bit7zLibrary lib(BIT7Z_STRING("7zip.dll"));
			// Opening the archive
			bit7z::BitArchiveReader archive{ lib, inArchive.string(), bit7z::BitFormat::Auto };
			// Verify archive integrity, throw BitException if invalid
			archive.test();
			// Finally: unpack to target folder
			archive.extractTo(outDir.string());
		}
		catch ([[maybe_unused]] const bit7z::BitException& ex) {
			return false;
		}
		return true;
	}

	auto inspectArchive(const fs::path& archivePath) -> std::optional<Archive::Information> {
		try {
			bit7z::Bit7zLibrary lib(BIT7Z_STRING("7zip.dll"));
			bit7z::BitArchiveReader arc{ lib, archivePath.string(), bit7z::BitFormat::Auto };
			return Archive::Information{
				.itemsCount = arc.itemsCount(),
				.foldersCount = arc.foldersCount(),
				.filesCount = arc.filesCount(),
				.packSize = arc.packSize(),
				.size = arc.size()
			};
		}
		catch ([[maybe_unused]] const bit7z::BitException& ex) {
			return std::nullopt;
		}
	}

	// Clean up stale temp files from crashed or interrupted runs
	bool cleanUpRemainingTempFiles()
	{
		bool isClean{ true }; // We want to get feedback if any of the cleanup calls fail
		std::error_code ec; // Fail silently, it's not a vital call
		return removeFileNoThrow(g_tempTextLocation, ec, isClean);
	}

	std::string calculateMd5HashFromFile(const fs::path& filename)
	{
		ssl::SslDigest digest{ ssl::SslDigest::Type::MD5 };
		std::ifstream file(filename, std::ios::binary);
		if (!file) {
			// Still possible to further split error cases up using std::filesystem::exists
			const std::error_code ec{ std::make_error_code(std::errc::io_error) };
			throw std::filesystem::filesystem_error("Failed to open file", filename, ec);
		}
		auto buffer{ std::make_unique_for_overwrite<char[]>(g_bufferSize) };
		while (file) {
			file.read(buffer.get(), g_bufferSize);
			std::streamsize bytesRead = file.gcount();
			digest.update(buffer.get(), bytesRead);
		}
		return digest.finalize();
	}

	bool compareMd5Hashes(std::string_view hash1, std::string_view hash2)
	{
		// TODO: remove case sensitivity and irrelevant characters
		return hash1.compare( hash2 ) == 0;
	}

	bool isFolder(const fs::path& path_in)
	{
		return fs::is_directory(path_in);
	}

	bool isFile(const fs::path& path_in)
	{
		return fs::is_regular_file(path_in);
	}

	// Write file to temp, remove current if necessary, copy temp to location
	void writeStringAsFile(const fs::path& filename, std::string_view filecontent)
	{
		ensureParentFolderExists(filename);
		TempFileJunkCollector tempFile{ g_tempTextLocation }; // Cleanup temp file in most usecases
		{
			std::ofstream tempTarget(tempFile.path, std::ios::out | std::ios::binary);
			tempTarget.exceptions(std::ios::failbit | std::ios::badbit);
			tempTarget << filecontent;
		} // tempTarget closed

		fs::copy_file(tempFile.path, filename, fs::copy_options::overwrite_existing);
	}

	std::string readFirstLineInFile(const fs::path& filename) {
		std::ifstream source(filename, std::ios::in);
		if (!source) {
			const std::error_code ec{ std::make_error_code(std::errc::io_error) };
			throw std::filesystem::filesystem_error("Failed to open file", filename, ec);
		}
		std::string out{};
		std::getline(source, out);
		return out;
	}

	std::string readTextFile(const fs::path& filename) {
		std::ifstream source(filename, std::ios::in | std::ios::binary);
		if (!source) {
			const std::error_code ec{ std::make_error_code(std::errc::io_error) };
			throw std::filesystem::filesystem_error("Failed to open file", filename, ec);
		}
		return { std::istreambuf_iterator<char>{source}, std::istreambuf_iterator<char>{} };
	}

	bool copyFileTo(const fs::path& filePath, const fs::path& targetPath, bool isClean) {
		std::error_code ec{};
		return copyFileTo(filePath, targetPath, ec, isClean);
	}

	bool copyFileTo(const fs::path& filePath, const fs::path& targetPath, std::error_code& ec, bool isClean) {
		return fs::copy_file(filePath, targetPath, fs::copy_options::overwrite_existing, ec) && isClean;
	}

	bool copyFolderInto(const fs::path& folderPath, const fs::path& targetPath, bool isClean) {
		std::error_code ec{};
		return copyFolderInto(folderPath, targetPath, ec, isClean);
	}

	bool copyFolderInto(const fs::path& folderPath, const fs::path& targetPath, std::error_code& ec, bool isClean) {
		fs::copy(folderPath, targetPath, fs::copy_options::overwrite_existing, ec);
		return verifyClean(isClean, ec);
	}

	bool createFolder(const fs::path& folderPath, bool isClean) noexcept {
		std::error_code ec{};
		return createFolder(folderPath, ec, isClean);
	}
	bool createFolder(const fs::path& folderPath, std::error_code&, bool isClean) noexcept {
		return fs::create_directory(folderPath) && isClean;  // Try to create directory even if isClean is already false
	}

	void removeFile(const fs::path& filename) {
		fs::remove(filename);
	}

	bool removeFileNoThrow(const fs::path& filename, bool isClean) noexcept {
		std::error_code ec{};
		return removeFileNoThrow(filename, ec, isClean);
	}

	bool removeFileNoThrow(const fs::path& filename, std::error_code& ec, bool isClean) noexcept {
		// fs::remove() returns true if there was a file, false if not. Both cases are fine with us
		fs::remove(filename, ec);
		// update "isClean" status indicator
		return verifyClean(isClean, ec);
	}

	std::uintmax_t removeFolderRecursively(const fs::path& filename) {
		std::error_code ec{};
		return removeFolderRecursively(filename, ec);
	}
	std::uintmax_t removeFolderRecursively(const fs::path& filename, std::error_code& ec) {
		return fs::remove_all(filename, ec);
	}

} // namespace Updater2::IO