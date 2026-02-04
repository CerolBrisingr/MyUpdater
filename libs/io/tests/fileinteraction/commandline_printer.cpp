#include <fstream>
#include <string_view>
#include <string>

constexpr std::string_view g_filename{ "output.txt" };

int main(int argc, char* argv[]) {
	std::ofstream target(g_filename.data(), std::ios::binary | std::ios::out);
	target << argv[0];
	for (int i{ 1 }; i < argc; ++i) {
		target << " " << argv[i];
	}
	return 0;
}