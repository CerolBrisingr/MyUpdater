#pragma once

#include <string>
#include <string_view>
#include <filesystem>
#include <format>

namespace fs = std::filesystem;

namespace Updater2::Certificates {

	class CertPair {
	public:
		CertPair(const fs::path& basePath, std::string_view filename, const std::error_code ec)
			: m_keyPath{ makePath(basePath, filename, "key", ec) }
			, m_certPath{ makePath(basePath, filename, "crt", ec) }
		{ }
		CertPair(CertPair&) = delete;
		CertPair(CertPair&&) = delete;
		CertPair operator=(CertPair&) = delete;
		CertPair operator=(CertPair&&) = delete;

		std::string key() const { return m_keyPath.string(); };
		std::string cert() const { return m_certPath.string(); };
		const fs::path& keyPath() const { return m_keyPath; };
		const fs::path& certPath() const { return m_certPath; };
	private:
		const fs::path m_keyPath{};
		const fs::path m_certPath{};

		static fs::path makePath(const fs::path& basePath, std::string_view filename, std::string_view extension, const std::error_code ec) 
		{
			if (ec) return fs::path();
			return basePath / std::format("{}.{}", filename, extension);
		}
	};

	class Handler {
		struct CertChecklist {
			bool hasCA{ false };
			bool hasServer{ false };
		};
	public:
		Handler(const fs::path& executable, const fs::path& caConfig, const fs::path& serverConfig);
		Handler(Handler&) = delete;
		Handler(Handler&&) = delete;
		Handler operator=(Handler&) = delete;
		Handler operator=(Handler&&) = delete;

		const CertPair& ca() const { return m_ca; };
		const CertPair& server() const { return m_server; };
	private:
		const fs::path m_opensslExecutable{};
		const fs::path m_caConfig{};
		const fs::path m_serverConfig{};
		std::error_code m_ec{};
		const fs::path m_currentPath{};
		const CertPair m_ca;
		const CertPair m_server;

		void buildCA() const;
		void createServerCert() const;

		void verifyEnvironment() const;
		Handler::CertChecklist queryExistingCertificates() const;
		void supplyCertificates(const Handler::CertChecklist& inventory) const;
		void verifyNewCerts(const Handler::CertChecklist& inventory) const;
		bool verifyCert(const CertPair& cert, const fs::path& configPath) const;
		bool verifyCert(const CertPair& cert) const;
	};
} // namespace Updater2::Certificates