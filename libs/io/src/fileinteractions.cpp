#include "IO/fileinteractions.h"

namespace fs = std::filesystem;

namespace Updater2::IO {

	std::string calculateMd5Hash(const std::string& filename)
	{
		SslDigest digest{ SslDigest::Type::MD5 };
		std::ifstream file(filename, std::ios::binary);
		if (!file) {
			return "";
		}
		auto buffer{ std::make_unique<char[]>(g_bufferSize) };
		while (file) {
			file.read(buffer.get(), g_bufferSize);
			std::streamsize bytesRead = file.gcount();
			digest.update(buffer.get(), bytesRead);
			std::cout << "Did read " << bytesRead << " bytes\n";
		}
		return digest.finalize();
	}

	bool compareMd5Hashes(std::string_view hash1, std::string_view hash2)
	{
		std::cout << "Hash 1: " << hash1 << '\n';
		std::cout << "Hash 2: " << hash2 << '\n';
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
		std::ifstream target(filename, std::ios::in);
		return "Not ready!";
	}

} // namespace Updater2::IO