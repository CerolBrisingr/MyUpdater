#include "Updater2/IO/filesystem.h"

namespace fs = std::filesystem;

namespace Updater2::IO {

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
	}

	std::string readFirstLineInFile(const std::string& filename) {
		std::ifstream target(filename, std::ios::in);
	}

} // namespace Updater2::IO