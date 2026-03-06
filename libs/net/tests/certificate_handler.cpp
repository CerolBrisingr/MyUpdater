#include "io/fileinteractions.h"
#include "certificate_handler.h"

#include <iostream>

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
		if (!verifyEnvironment()) return;
		auto inventory = queryExistingCertificates();
		if (!supplyCertificates(inventory)) return;
		verifyNewCerts(inventory);
	}

	bool Handler::setError(std::string_view error_msg) {
		m_state.error_msg = error_msg;
		m_state.isValid = false;
		return false;
	}

	bool Handler::verifyEnvironment() {
		if (m_ec) {
			return setError(std::format("Failed to determine current path: '{}'", m_ec.message()));
		}
		if (!myfs::createFolder(m_currentPath / "certs")) {
			return setError("Failed to establish target folder 'certs'");
		}
		if (!myfs::isFile(m_opensslExecutable)) {
			return setError(std::format("OpenSSL executable not found at '{}'", m_opensslExecutable.string()));
		}
		if (!myfs::isFile(m_caConfig)) {
			return setError(std::format("Missing CA configuration file at '{}'", m_caConfig.string()));
		}
		if (!myfs::isFile(m_serverConfig)) {
			return setError(std::format("Missing server configuration file at '{}'", m_serverConfig.string()));
		}
		return true;
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

	bool Handler::supplyCertificates(const Handler::CertChecklist& inventory) {
		if (!inventory.hasCA) {
			if (!buildCA()) {
				return setError("Root certificate creation returned error");
			}
		}
		if (!inventory.hasServer) {
			if (!createServerCert()) {
				return setError("Server certificate creation returned error");
			}
		}
		return true;
	}

	void Handler::verifyNewCerts(const Handler::CertChecklist& inventory) {
		if (!inventory.hasCA && !verifyCert(m_ca, m_caConfig)) {
			setError("Root certificate was not available and could not be created");
			return;
		}
		if (!inventory.hasServer && !verifyCert(m_server, m_serverConfig)) {
			setError("Server certificate was not available and could not be created");
			return;
		}
	}

	bool Handler::buildCA() {
		const myfs::stringList arguments{
			"req", "-x509", "-new", "-noenc",
			"-keyout", m_ca.key(),
			"-out", m_ca.cert(), "-sha256", "-days", "3650",
			"-config", m_caConfig.string()
		};
		return myfs::createProcess(m_opensslExecutable, arguments, true);
	}

	bool Handler::createServerCert() {
		const fs::path serial{ m_currentPath / "certs/server.csr" };
		const myfs::stringList keyArguments{
			"req", "-new", "-out", serial.string(),
			"-newkey", "rsa:2048", "-noenc", "-keyout", m_server.key(),
			"-config", m_serverConfig.string()
		};
		bool serverKeyOk{ myfs::createProcess(m_opensslExecutable, keyArguments, true) };

		const myfs::stringList certArguments{
			"x509", "-req", "-in", serial.string(),
			"-CA", m_ca.cert(), "-CAkey", m_ca.key(), "-CAcreateserial",
			"-out", m_server.cert(), "-days", "15", "-sha256",
			"-copy_extensions", "copy"
		};
		return serverKeyOk && myfs::createProcess(m_opensslExecutable, certArguments, true);

	}

	bool Handler::verifyCert(const CertPair& cert, const fs::path& configPath) const {
		if (!myfs::isFile(cert.certPath()) || !myfs::isFile(configPath)) {
			return false;	// Missing file
		}
		if (!myfs::file1IsOlderThan2(configPath, cert.certPath())) {
			return false;	// Config file changed since certificate was created
		}
		return verifyCert(cert);
	}

	bool Handler::verifyCert(const CertPair& cert) const {
		if (!myfs::isFile(cert.certPath()) || !myfs::isFile(cert.keyPath())) {
			return false;	// Missing file
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
			return false;	// Failed to probe key
		}
		if (!myfs::isFile(tempOutput)) {
			std::cout << "Failed to write output file for key modulus!\n";
			return false;
		}
		std::string keyModulus{ myfs::readTextFile(tempOutput) };

		// Take modulus of certificate file
		const myfs::stringList x509ModulusArguments{ "x509", "-noout", "-modulus", "-in", cert.cert(), "-out", tempOutput.string()};
		if (!myfs::createProcess(m_opensslExecutable, x509ModulusArguments, true)) {
			return false;	// Failed to probe certificate
		}
		if (!myfs::isFile(tempOutput)) {
			std::cout << "Failed to write output file for cert modulus!\n";
			return false;
		}
		std::string certModulus{ myfs::readTextFile(tempOutput) };

		myfs::removeFile(tempOutput); // Clean up temp file
		return keyModulus == certModulus; // Return true if key and certificate match
	}
} // namespace Updater2::Certificates