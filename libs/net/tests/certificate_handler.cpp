#include "io/fileinteractions.h"
#include "certificate_handler.h"

namespace Updater2::Certificates {
	Handler::Handler(std::string_view executable)
		: m_opensslExecutable{ executable }
	{

	}
} // namespace Updater2::Certificates