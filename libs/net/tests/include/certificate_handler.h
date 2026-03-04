#pragma once

#include <string>
#include <string_view>

namespace Updater2::Certificates {
	class Handler {
	public:
		Handler(std::string_view executable);
	private:
		const std::string m_opensslExecutable{};
	};
} // namespace Updater2::Certificates