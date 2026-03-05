#pragma once

#include <string>
#include <string_view>
#include <filesystem>
#include <format>

namespace fs = std::filesystem;

namespace Updater2::Certificates {

	class CertPair {
	public:
		CertPair(const fs::path& basePath, std::string_view filename)
			: m_keyPath { basePath / std::format("{}.key", filename) }
			, m_certPath{ basePath / std::format("{}.crt", filename) }
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
	};

	class Handler {
	public:


		Handler(const fs::path& executable, const fs::path& caConfig, const fs::path& serverConfig);
		Handler(Handler&) = delete;
		Handler(Handler&&) = delete;
		Handler operator=(Handler&) = delete;
		Handler operator=(Handler&&) = delete;
	private:
		const fs::path m_opensslExecutable{};
		const fs::path m_caConfig{};
		const fs::path m_serverConfig{};
		std::error_code m_ec{};
		const fs::path m_currentPath{};
	public:
		// Those need to be initialized later than the base path values
		const CertPair m_ca;
		const CertPair m_server;
	private:

		bool buildCA();
		bool createServerCert();

		bool verifyCert(const CertPair& cert, const fs::path& configPath);
		bool verifyCert(const CertPair& cert);
	};
} // namespace Updater2::Certificates