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

} // namespace Updater2::IO