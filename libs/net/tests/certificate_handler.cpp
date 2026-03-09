#include "io/fileinteractions.h"
#include "certificate_handler.h"

#include <stdexcept>

namespace myfs = Updater2::IO;

namespace Updater2::Certificates {
	Handler::Handler(const fs::path& executable, const fs::path& caConfig, const fs::path& serverConfig)
		: m_opensslExecutable{ executable }
		, m_caConfig{ caConfig }
		, m_serverConfig{ serverConfig }
		, m_currentPath{ fs::current_path(m_ec)}
		, m_ca{ m_currentPath / "certs", "ca" , m_ec }
		, m_server{ m_currentPath / "certs", "server", m_ec }
		, m_timeoutServer{ m_currentPath / "certs", "timeout_server", m_ec }
		, m_selfCertServer{ m_currentPath / "certs", "self_cert_server", m_ec }
	{
		verifyEnvironment();
		auto inventory = queryExistingCertificates();
		supplyCertificates(inventory);
		verifyNewCerts(inventory);
	}

	void Handler::verifyEnvironment() const {
		if (m_ec) {
			throw std::runtime_error(std::format("Failed to determine current path: '{}'", m_ec.message()));
		}
		if (!myfs::createFolder(m_currentPath / "certs")) {
			throw std::runtime_error("Failed to establish target folder 'certs'");
		}
		if (!myfs::isFile(m_opensslExecutable)) {
			throw std::runtime_error(std::format("OpenSSL executable not found at '{}'", m_opensslExecutable.string()));
		}
		if (!myfs::isFile(m_caConfig)) {
			throw std::runtime_error(std::format("Missing CA configuration file at '{}'", m_caConfig.string()));
		}
		if (!myfs::isFile(m_serverConfig)) {
			throw std::runtime_error(std::format("Missing server configuration file at '{}'", m_serverConfig.string()));
		}
	}

	Handler::CertChecklist Handler::queryExistingCertificates() const {
		CertChecklist inventory{};
		if (verifyCert(m_ca, m_caConfig)) {
			inventory.hasCA = true;
			// Now test dependant certificates
			if (verifyCert(m_server, m_serverConfig)) {
				inventory.hasServer = true;
			}
			if (verifyCert(m_timeoutServer, m_serverConfig, CertTypes::TIMEOUT)) {
				inventory.hasTimeoutServer = true;
			}
		}
		return inventory;
	}

	void Handler::supplyCertificates(const Handler::CertChecklist& inventory) const {
		if (!inventory.hasCA) {
			buildCA();
		}
		if (!inventory.hasServer) {
			const fs::path serial{ m_currentPath / "certs/server.csr" };
			createServerCert(m_server, serial, "15");
		}
		if (!inventory.hasTimeoutServer) {
			const fs::path serial{ m_currentPath / "certs/timeout_server.csr" };
			createServerCert(m_timeoutServer, serial, "0");
		}
	}

	void Handler::verifyNewCerts(const Handler::CertChecklist& inventory) const {
		if (!inventory.hasCA && !verifyCert(m_ca, m_caConfig)) {
			throw std::runtime_error("Valid root certificate was not available and could not be created");
		}
		if (!inventory.hasServer && !verifyCert(m_server, m_serverConfig)) {
			throw std::runtime_error("Valid server certificate was not available and could not be created");
		}
		if (!inventory.hasTimeoutServer && !verifyCert(m_timeoutServer, m_serverConfig, CertTypes::TIMEOUT)) {
			throw std::runtime_error("Valid timed-out server certificate was not available and could not be created");
		}
	}

	void Handler::buildCA() const {
		const myfs::stringList arguments{
			"req", "-x509", "-new", "-noenc",
			"-keyout", m_ca.key(),
			"-out", m_ca.cert(), "-sha256", "-days", "3650",
			"-config", m_caConfig.string()
		};
		if (!myfs::createProcess(m_opensslExecutable, arguments, true)) {
			throw std::runtime_error("Root certificate creation returned error");
		}
	}

	void Handler::createServerCert(const CertPair& server, const fs::path& serialPath, const std::string& days) const {
		createServerKey(server, serialPath, days);
		certifyServer(server, serialPath, days);
	}

	void Handler::createServerKey(const CertPair& server, const fs::path& serialPath, const std::string& days) const {
		const myfs::stringList keyArguments{
			"req", "-new", "-out", serialPath.string(),
			"-newkey", "rsa:2048", "-noenc", "-keyout", server.key(),
			"-config", m_serverConfig.string()
		};
		if (!myfs::createProcess(m_opensslExecutable, keyArguments, true)) {
			throw std::runtime_error(std::format("Server key creation ({}days) returned error", days));
		}
	}

	void Handler::certifyServer(const CertPair& server, const fs::path& serialPath, const std::string& days) const {
		const myfs::stringList certArguments{
			"x509", "-req", "-in", serialPath.string(),
			"-CA", m_ca.cert(), "-CAkey", m_ca.key(), "-CAcreateserial",
			"-out", server.cert(), "-days", days, "-sha256",
			"-copy_extensions", "copy"
		};
		if (!myfs::createProcess(m_opensslExecutable, certArguments, true)) {
			throw std::runtime_error(std::format("Server certification ({}days) returned error", days));
		}
	}

	bool Handler::verifyCert(const CertPair& cert, const fs::path& configPath, CertTypes mode) const {
		if (!myfs::isFile(cert.certPath())) {
			return false;	// Certificate missing, no need to check
		}
		if (!myfs::file1IsOlderThan2(configPath, cert.certPath())) {
			return false;	// Config file changed since certificate was created
		}
		return verifyCert(cert, mode);
	}

	bool Handler::verifyCert(const CertPair& cert, const CertTypes mode) const {
		if (!myfs::isFile(cert.certPath()) || !myfs::isFile(cert.keyPath())) {
			return false;	// Missing file(s), no need to test them
		}

		const CertPair* reference = &m_ca;
		if (mode == CertTypes::SELFCERT) {
			reference = &cert;
		}
		// Test for timeout of certificate
		if (mode == CertTypes::TIMEOUT) {
			// Timeout needs to be in effect NOW
			if (isStillValid(*reference, cert, 0)) {
				return false;
			}
		}
		else {
			// We need at least enough time to do our tests (default is 2d)
			if (!isStillValid(*reference, cert)) {
				return false;
			}
		}

		// Select file for text output
		const static fs::path tempOutput{ m_currentPath / "certs/temp_modulus.txt" };

		// Take modulus of key file
		const myfs::stringList rsaModulusArguments{ "rsa", "-noout", "-modulus", "-in", cert.key(), "-out", tempOutput.string()};
		if (!myfs::createProcess(m_opensslExecutable, rsaModulusArguments, true)) {
			throw std::runtime_error("Failed to query key modulus");
		}
		if (!myfs::isFile(tempOutput)) {
			throw std::runtime_error("Failed to write output file for key modulus");
		}
		std::string keyModulus{ myfs::readTextFile(tempOutput) };

		// Take modulus of certificate file
		const myfs::stringList x509ModulusArguments{ "x509", "-noout", "-modulus", "-in", cert.cert(), "-out", tempOutput.string()};
		if (!myfs::createProcess(m_opensslExecutable, x509ModulusArguments, true)) {
			throw std::runtime_error("Failed to query certificate modulus");
		}
		if (!myfs::isFile(tempOutput)) {
			throw std::runtime_error("Failed to write output file for cert modulus");
		}
		std::string certModulus{ myfs::readTextFile(tempOutput) };

		myfs::removeFile(tempOutput); // Clean up temp file
		return keyModulus == certModulus; // Return true if key and certificate modulus match
	}

	bool Handler::isStillValid(const CertPair& ca, const CertPair& cert, int leewayDays) const {
		// Test certificate for validity. -checkend behavior is platform dependant in cases where expiry is in the past
		const myfs::stringList runtimeArguments{ "verify", "-CAfile", ca.cert(), cert.cert()};
		if (!myfs::createProcess(m_opensslExecutable, runtimeArguments, true)) {
			return false;
		}
		// Default: Less than 2 days (2x 62400s) remaining is too little.
		if (leewayDays > 0) {
			const myfs::stringList checkendArgs{ "x509", "-checkend", std::to_string(leewayDays * 62400), "-noout", "-in", cert.cert()};
			const bool hasEnoughBuffer = myfs::createProcess(m_opensslExecutable, checkendArgs, true);
			return hasEnoughBuffer;
		}
		return true;
	}

} // namespace Updater2::Certificates