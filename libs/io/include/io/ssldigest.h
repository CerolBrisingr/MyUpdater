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

		// This interface begrudgingly uses X-Macros
		// 1) Single source of truth for both enum "Type" and array "digestNames"
		// 2) No leaking sentinel as used in "static_assert(std::size(digestNames) == static_cast<std::size_t>(Type::numTypes));"
#define DIGEST_LIST(DO) \
	DO(MD5, "md5"sv) \
	DO(SHA2_224, "sha224"sv) \
	DO(SHA2_256, "sha256"sv) \
	DO(SHA2_512_224, "sha512-224"sv) \
	DO(SHA2_512_256, "sha512-256"sv) \
	DO(SHA2_384, "sha384"sv) \
	DO(SHA3_224, "sha3-224"sv) \
	DO(SHA3_256, "sha3-256"sv) \
	DO(SHA3_384, "sha3-384"sv) \
	DO(SHA3_512, "sha3-512"sv)

#define PRODUCE_TAG(tag, name) tag,
#define PRODUCE_NAME(tag, name) name, 
		enum class Type {
			DIGEST_LIST(PRODUCE_TAG)
		}; // First prints out content of digest_list while replacing "do" with "produce_tag", then the other macro reduces each occurence to "tag,"
		static constexpr std::array digestNames{
			DIGEST_LIST(PRODUCE_NAME)
		};
#undef DIGEST_LIST
#undef PRODUCE_TAG
#undef PRODUCE_NAME

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
