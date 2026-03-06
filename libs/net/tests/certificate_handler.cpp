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
		, m_ca{ m_currentPath / "certs", "ca" , m_ec}
		, m_server{ m_currentPath / "certs", "server", m_ec}
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
		}
		else {
			// If we need a new CA certificate also the server 
			// certificate needs to be renewed
			return inventory;
		}
		if (verifyCert(m_server, m_serverConfig)) {
			inventory.hasServer = true;
		}
		return inventory;
	}

	void Handler::supplyCertificates(const Handler::CertChecklist& inventory) const {
		if (!inventory.hasCA) {
			buildCA();
		}
		if (!inventory.hasServer) {
			createServerCert();
		}
	}

	void Handler::verifyNewCerts(const Handler::CertChecklist& inventory) const {
		if (!inventory.hasCA && !verifyCert(m_ca, m_caConfig)) {
			throw std::runtime_error("Valid root certificate was not available and could not be created");
		}
		if (!inventory.hasServer && !verifyCert(m_server, m_serverConfig)) {
			throw std::runtime_error("Valid server certificate was not available and could not be created");
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

	void Handler::createServerCert() const {
		const fs::path serial{ m_currentPath / "certs/server.csr" };
		const myfs::stringList keyArguments {
			"req", "-new", "-out", serial.string(),
			"-newkey", "rsa:2048", "-noenc", "-keyout", m_server.key(),
			"-config", m_serverConfig.string()
		};
		if (!myfs::createProcess(m_opensslExecutable, keyArguments, true)) {
			throw std::runtime_error("Server key creation returned error");
		}

		const myfs::stringList certArguments {
			"x509", "-req", "-in", serial.string(),
			"-CA", m_ca.cert(), "-CAkey", m_ca.key(), "-CAcreateserial",
			"-out", m_server.cert(), "-days", "15", "-sha256",
			"-copy_extensions", "copy"
		};
		if (!myfs::createProcess(m_opensslExecutable, certArguments, true)) {
			throw std::runtime_error("Server certification returned error");
		}

	}

	bool Handler::verifyCert(const CertPair& cert, const fs::path& configPath) const {
		if (!myfs::isFile(cert.certPath())) {
			return false;	// Certificate missing, no need to check
		}
		if (!myfs::file1IsOlderThan2(configPath, cert.certPath())) {
			return false;	// Config file changed since certificate was created
		}
		return verifyCert(cert);
	}

	bool Handler::verifyCert(const CertPair& cert) const {
		if (!myfs::isFile(cert.certPath()) || !myfs::isFile(cert.keyPath())) {
			return false;	// Missing file(s), no need to test them
		}
		// Test certificate for remaining runtime. Less than 2 days (172800s) is too little.
		const myfs::stringList runtimeArguments{ "x509", "-checkend", "172800", "-noout", "-in", cert.cert() };
		if (!myfs::createProcess(m_opensslExecutable, runtimeArguments, true)) {
			return false;	// Certificate no longer valid (or at least 2 days from now)
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
		return keyModulus == certModulus; // Return true if key and certificate match
	}
} // namespace Updater2::Certificates