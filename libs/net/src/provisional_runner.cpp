#include "downloader.h"

#include <sstream>
#include <iostream>

using namespace Updater2;

int main()
{
	std::ostringstream target{};
	Downloader::fetch(target, "https://www.example.com");

	std::cout << "Reached end of fetch!\n";
	std::cout << "Showing received string:\n" << target.str();
	return 0;
}