#include "IO/fileinteractions.h"

namespace fs = std::filesystem;

namespace Updater2::IO {

	std::string readMd5Hash(const std::string& filename)
	{
		std::cout << "Will calculate md5 hash for file: " << filename << '\n';
		//MD5_CTX* test;
		//MD5_Init(test);
		std::cout << "Arrived here!\n";
		return "";
	}

	void printCurrentPath()
	{
		std::cout << "Current path is: \"" << fs::current_path() << "\"\n";
	}

	bool isFolder(std::string_view path_in)
	{
		return fs::is_directory(path_in);
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