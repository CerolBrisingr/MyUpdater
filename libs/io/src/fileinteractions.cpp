#include "IO/fileinteractions.h"

namespace ssl = Updater2::SSL;
namespace fs = std::filesystem;

namespace Updater2::IO {

	std::string calculateMd5HashFromFile(const std::string& filename)
	{
		ssl::SslDigest digest{ ssl::SslDigest::Type::MD5 };
		std::ifstream file(filename, std::ios::binary);
		if (!file) {
			// Still possible to further split this up using std::filesystem::exists
			const std::error_code ec{ std::make_error_code(std::errc::io_error) };
			throw std::filesystem::filesystem_error("Failed to open file", filename, ec);
		}
		auto buffer{ std::make_unique<char[]>(g_bufferSize) };
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

	bool writeStringAsFile(const std::string &filename, std::string_view filecontent)
	{
		std::ofstream target( filename, std::ios::out);
		if (target) {
			target << filecontent;
		}
		else {
			return false;
		}
		return true;
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

} // namespace Updater2::IO