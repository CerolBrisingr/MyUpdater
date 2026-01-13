#pragma once

#include <openssl/evp.h>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <array>
#include <cstddef>
#include <string>
#include <string_view>

using namespace std::literals;

namespace Updater2::IO {

	static constexpr std::size_t g_bufferSize{ 1024 * 1024 };  // 1MB

	class SslDigest {
	public:
		enum class Type { MD5, SHA2_224, SHA2_256, SHA2_512_224, SHA2_512_256, SHA2_384, SHA3_224, SHA3_256, SHA3_384, SHA3_512, numTypes };
		static constexpr std::array digestNames{ "md5"sv, "sha224"sv, "sha256"sv, "sha512-224"sv,
			"sha512-256"sv, "sha384"sv, "sha3-224"sv, "sha3-256"sv, "sha3-384"sv, "sha3-512"sv };
		static_assert(std::size(digestNames) == static_cast<std::size_t>(Type::numTypes));

		explicit SslDigest(SslDigest::Type digestType);
		~SslDigest() noexcept;

		SslDigest(const SslDigest&) = delete;
		SslDigest& operator= (const SslDigest&) = delete;

		SslDigest(SslDigest&& rhs) noexcept;
		SslDigest& operator= (SslDigest&& rhs) noexcept;

		bool isValid() const;
		void update(const void* data, std::size_t count);
		std::string finalize();
	private:
		EVP_MD_CTX* mdctx = nullptr; // Context for EVP (State & Settings I'd expect)
		std::string getEvpMdName(SslDigest::Type digestType);
		std::string getHexString(unsigned char* data, unsigned int numElements);
	};
} // namespace Updater2::IO
