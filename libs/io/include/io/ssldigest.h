#pragma once

#include <openssl/evp.h>
#include <openssl/provider.h>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <array>
#include <cstddef>
#include <string>
#include <stdexcept>
#include <memory>

namespace Updater2::SSL {

	constexpr const char* move_on_error{ "This object was most likely moved - from, initialize before re - use" };

	class SslDigest {
	private:
		// Helper must be declared ahead of Macro expansion
		static const EVP_MD* getMd5();
	public:		// enum class Type needs to be exposed
		// This interface begrudgingly uses X-Macros
		// 1) Single source of truth for both enum "Type" and array "digestMds"
		// 2) No leaking sentinel as used in "static_assert(std::size(digestMds) == static_cast<std::size_t>(Type::numTypes));"
#define DIGEST_LIST(DO) \
	DO(MD5, &getMd5) \
	DO(SHA2_224, &EVP_sha224) \
	DO(SHA2_256, &EVP_sha256) \
	DO(SHA2_512_224, &EVP_sha512_224) \
	DO(SHA2_512_256, &EVP_sha512_256) \
	DO(SHA2_384, &EVP_sha384) \
	DO(SHA3_224, &EVP_sha3_224) \
	DO(SHA3_256, &EVP_sha3_256) \
	DO(SHA3_384, &EVP_sha3_384) \
	DO(SHA3_512, &EVP_sha3_512)

#define PRODUCE_TAG(tag, fn) tag,
#define PRODUCE_FN(tag, fn) fn,
		enum class Type {
			DIGEST_LIST(PRODUCE_TAG)
		}; // First prints out content of digest_list while replacing "do" with "produce_tag", then macro "produce_tag" reduces each occurence to "tag,"
	private: // digestMds does not need to be exposed
		static inline constexpr std::array digestMds{
			DIGEST_LIST(PRODUCE_FN)
		};
#undef DIGEST_LIST
#undef PRODUCE_TAG
#undef PRODUCE_NAME
	public: // Ugly stuff is done.

		explicit SslDigest(SslDigest::Type digestType);
		~SslDigest() noexcept = default;

		SslDigest(const SslDigest& rhs);
		SslDigest& operator= (const SslDigest& rhs);

		SslDigest(SslDigest&& rhs) noexcept;
		SslDigest& operator= (SslDigest&& rhs) noexcept;

		void update(const void* data, std::size_t count);
		std::string finalize();

		friend void swap(SslDigest& a, SslDigest& b) noexcept;
	private:
		// g_mdctx: Context for EVP (State & Settings I'd expect)
		std::unique_ptr<EVP_MD_CTX, decltype(&EVP_MD_CTX_free)> g_mdctx{ nullptr, &EVP_MD_CTX_free };

		const EVP_MD* getEvpMd(SslDigest::Type digestType);
		std::string getHexString(unsigned char* data, unsigned int numElements);
		void restoreDigestContext();
	};

	void swap(SslDigest& a, SslDigest& b) noexcept;

} // namespace Updater2::IO
