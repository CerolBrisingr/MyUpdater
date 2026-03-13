#include <fstream>
#include <string_view>
#include <string>
#include <cstring>

constexpr std::string_view g_filename{ COMMANDLINE_PRINTER_FILE };

int main(int argc, char* argv[]) {
	std::ofstream target(g_filename.data(), std::ios::binary | std::ios::out);
	target << argv[0];
	for (int i{ 1 }; i < argc; ++i) {
		target << " " << argv[i];
	}
	if ((argc == 2) && (std::strcmp(argv[1], "-dofail") == 0)) {
		return 1;
	}
	return 0;
}