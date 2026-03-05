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
		, m_ca{ m_currentPath / "certs", "ca" }
		, m_server{ m_currentPath / "certs", "server"}
	{
		if (m_ec) return;		// Cannot continue without current path
		bool success = myfs::createFolder(m_currentPath / "certs");
		if (!success) return;	// Cannot continue without target directory
		if (buildCA()) {
			std::cout << "CA created\n";
			if (createServerCert()) {
				std::cout << "Server certificate created\n";
			}
		}

		if (verifyCert(m_ca, m_caConfig)) {
			std::cout << "CA certificate verified!\n";
		}
		if (verifyCert(m_server, m_serverConfig)) {
			std::cout << "Server certificate verified!\n";
		}
	}

	bool Handler::buildCA() {
		/*    
		COMMAND ${OPENSSL_EXECUTABLE} req -x509 -new -noenc 
            -keyout ca.key -out ca.crt -sha256 -days 3650 
            -config "${CERT_FOLDER}/ca.conf"*/
		const myfs::stringList arguments{
			"req", "-x509", "-new", "-noenc",
			"-keyout", m_ca.key(),
			"-out", m_ca.cert(), "-sha256", "-days", "3650",
			"-config", m_caConfig.string()
		};
		return myfs::createProcess(m_opensslExecutable, arguments, true);
	}

	bool Handler::createServerCert() {
		/*    
		COMMAND ${OPENSSL_EXECUTABLE} req -new -out server.csr
            -newkey rsa:2048 -noenc -keyout server.key
            -config "${CERT_FOLDER}/server.conf"
		COMMAND ${OPENSSL_EXECUTABLE} x509 -req -in server.csr 
            -CA "${SSL_ASSET_CA_CERT}" -CAkey "${SSL_ASSET_CA_KEY}" -CAcreateserial 
            -out server.crt -days 15 -sha256 
            -copy_extensions copy*/
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

	bool Handler::verifyCert(const CertPair& cert, const fs::path& configPath) {
		if (!myfs::isFile(cert.certPath()) || !myfs::isFile(configPath)) {
			return false;	// Missing file
		}
		if (!myfs::file1IsOlderThan2(configPath, cert.certPath())) {
			return false;	// Config file changed since certificate was created
		}
		return verifyCert(cert);
	}

	bool Handler::verifyCert(const CertPair& cert) {
		if (!myfs::isFile(cert.certPath()) || !myfs::isFile(cert.keyPath())) {
			return false;	// Missing file
		}
		// COMMAND ${ OPENSSL_EXE } x509 - checkend 172800 - noout - in ${ C_PATH }
		const myfs::stringList runtimeArguments{ "x509", "-checkend", "172800", "-noout", "-in", cert.cert() };
		if (!myfs::createProcess(m_opensslExecutable, runtimeArguments, true)) {
			return false;	// Certificate no longer valid (or at least 2 days from now)
		}

		// Select file for text output
		const static fs::path tempOutput{ m_currentPath / "certs/temp_modulus.txt" };

		// COMMAND ${OPENSSL_EXE} rsa -noout -modulus -in ${K_PATH}
		const myfs::stringList rsaModulusArguments{ "rsa", "-noout", "-modulus", "-in", cert.key(), "-out", tempOutput.string()};
		if (!myfs::createProcess(m_opensslExecutable, rsaModulusArguments, true)) {
			return false;	// Failed to probe key
		}
		if (!myfs::isFile(tempOutput)) {
			std::cout << "Failed to write output file 1!\n";
			return false;
		}
		std::string keyModulus{ myfs::readTextFile(tempOutput) };

		// COMMAND ${OPENSSL_EXE} x509 -noout -modulus -in ${C_PATH}
		const myfs::stringList x509ModulusArguments{ "x509", "-noout", "-modulus", "-in", cert.cert(), "-out", tempOutput.string()};
		if (!myfs::createProcess(m_opensslExecutable, x509ModulusArguments, true)) {
			return false;	// Failed to probe certificate
		}
		if (!myfs::isFile(tempOutput)) {
			std::cout << "Failed to write output file 2!\n";
			return false;
		}
		std::string certModulus{ myfs::readTextFile(tempOutput) };

		myfs::removeFile(tempOutput);

		return keyModulus == certModulus; // Return true if key and certificate match
	}
} // namespace Updater2::Certificates