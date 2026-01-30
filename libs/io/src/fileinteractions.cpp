#include "IO/fileinteractions.h"

namespace ssl = Updater2::SSL;
namespace fs = std::filesystem;

namespace Updater2::IO {

	namespace {
		constexpr std::string_view g_tempTextLocation{ "temp.txt" };
		constexpr std::size_t g_bufferSize{ 1024 * 1024 };  // 1MB

		struct TempFileJunkCollector {
			const char* path{ nullptr };
			explicit TempFileJunkCollector(const char* p)
				:path{ p }
			{}

			TempFileJunkCollector(TempFileJunkCollector& rhs) = delete;
			TempFileJunkCollector& operator= (TempFileJunkCollector& rhs) = delete;

			void release()
			{
				path = nullptr;
			}

			~TempFileJunkCollector() {
				std::error_code ec;
				fs::remove(path, ec);
			}
		};
	} // namespace

	bool unzipArchive(const fs::path& inArchive, const fs::path& outDir)
	{
		try { // bit7z classes can throw BitException objects
			bit7z::Bit7zLibrary lib(BIT7Z_STRING("7zip.dll"));
			// Opening the archive
			bit7z::BitExtractor<bit7z::tstring> extractor{ lib, bit7z::BitFormat::Auto };
			// We can swap to .native() if bit7z is compiled with BIT7Z_USE_NATIVE_STRING
			//    and BIT7Z_USE_NATIVE_STRING is defined
			extractor.extract(inArchive.string(), outDir.string());
		}
		catch (const bit7z::BitException& ex) {
			return false;
		}
		return true;
	}

	// Clean up stale temp files from crashed or interrupted runs
	bool cleanUpRemainingTempFiles()
	{
		bool isClean{ true }; // We want to get feedback if any of the cleanup calls fail
		std::error_code ec; // Fail silently, it's not a vital call
		removeFileNoThrow(g_tempTextLocation, ec, isClean);
		return isClean;
	}

	std::string calculateMd5HashFromFile(const std::string& filename)
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

	void printCurrentPath()
	{
		std::cout << "Current path is: \"" << fs::current_path() << "\"\n";
	}

	bool isFolder(std::string_view path_in)
	{
		return fs::is_directory(path_in);
	}

	bool isFile(std::string_view path_in)
	{
		return fs::is_regular_file(path_in);
	}

	// Write file to temp, remove current if necessary, copy temp to location
	void writeStringAsFile(std::string_view filename, std::string_view filecontent)
	{
		TempFileJunkCollector jc{ g_tempTextLocation.data() }; // Cleanup temp file in most usecases
		{
			std::ofstream tempTarget(g_tempTextLocation.data(), std::ios::out);
			tempTarget.exceptions(std::ios::failbit | std::ios::badbit);
			tempTarget << filecontent;
		} // tempTarget closed

		fs::copy_file(g_tempTextLocation, filename, fs::copy_options::overwrite_existing);
	}

	std::string readFirstLineInFile(const std::string& filename) {
		std::ifstream source(filename, std::ios::in);
		if (!source) {
			const std::error_code ec{ std::make_error_code(std::errc::io_error) };
			throw std::filesystem::filesystem_error("Failed to open file", filename, ec);
		}
		std::string out{};
		std::getline(source, out);
		return out;
	}

	std::string readTextFile(const std::string& filename) {
		std::ifstream source(filename, std::ios::in | std::ios::binary);
		if (!source) {
			const std::error_code ec{ std::make_error_code(std::errc::io_error) };
			throw std::filesystem::filesystem_error("Failed to open file", filename, ec);
		}
		return { std::istreambuf_iterator<char>{source}, std::istreambuf_iterator<char>{} };
	}

	void removeFile(std::string_view filename) {
		fs::remove(filename);
	}

	bool removeFileNoThrow(std::string_view filename, bool isClean) noexcept {
		std::error_code ec{};
		return removeFileNoThrow(filename, ec, isClean);
	}

	bool removeFileNoThrow(std::string_view filename, std::error_code& ec, bool isClean) noexcept {
		// fs::remove() returns true if there was a file, false if not. Both cases are fine with us
		fs::remove(filename, ec);
		// update "isClean" status indicator
		if (ec) {
			isClean = false;
		}
		return isClean;
	}

} // namespace Updater2::IO