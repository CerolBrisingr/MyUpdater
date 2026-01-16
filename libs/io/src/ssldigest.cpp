#include "IO/ssldigest.h"

namespace Updater2::IO {

	SslDigest::SslDigest(Type digestType)
	{
		const EVP_MD* digestAlgorithm{ getEvpMd(digestType) };
		if (!digestAlgorithm) throw std::logic_error("Requested digest is not available");

		g_mdctx.reset(EVP_MD_CTX_new());
		if (!g_mdctx) throw std::bad_alloc();

		if (EVP_DigestInit_ex(g_mdctx.get(), digestAlgorithm, nullptr) != 1)
		{
			throw std::runtime_error("EVP_DigestInit_ex failed");
		}
	}

	SslDigest::~SslDigest() {}

	SslDigest::SslDigest(const SslDigest& rhs) 
	{
		g_mdctx.reset(EVP_MD_CTX_new());
		if (g_mdctx == nullptr) throw std::bad_alloc();
		if (EVP_MD_CTX_copy_ex(g_mdctx.get(), rhs.g_mdctx.get()) != 1)
		{
			throw std::runtime_error("EVP_MD_CTX_copy_ex failed");
		}
	}

	SslDigest& SslDigest::operator= (const SslDigest& rhs) 
	{
		if (this == &rhs) return *this;
		SslDigest tmp{ rhs };
		swap(*this, tmp);
		return *this;
	}

	SslDigest::SslDigest(SslDigest&& rhs) noexcept
		:g_mdctx{ std::move(rhs.g_mdctx) }
	{}

	SslDigest& SslDigest::operator= (SslDigest&& rhs) noexcept
	{
		if (&rhs == this) { return *this; }
		g_mdctx.swap(rhs.g_mdctx);
		return *this;
	}

	void SslDigest::update(const void* data, std::size_t count) {
		if (!g_mdctx) [[unlikely]] {
			throw std::runtime_error("This object was most likely moved-from, initialize before re-use");
		}
		if (EVP_DigestUpdate(g_mdctx.get(), data, count) != 1) {
			throw std::runtime_error("EVP_DigestUpdate failed");
		}
	}

	std::string SslDigest::finalize() {
		if (!g_mdctx) [[unlikely]] {
			throw std::runtime_error("This object was most likely moved-from, initialize before re-use");
		}
		unsigned char md_value[EVP_MAX_MD_SIZE];
		unsigned int md_len;
		if (EVP_DigestFinal_ex(g_mdctx.get(), md_value, &md_len) != 1) {
			throw std::runtime_error("EVP_DigestFinal_ex failed");
		}
		restoreDigestContext();
		return getHexString(md_value, md_len);
	}

	void SslDigest::restoreDigestContext()
	{
		if (EVP_DigestInit_ex(g_mdctx.get(), nullptr, nullptr) != 1)
		{
			throw std::runtime_error("EVP_DigestInit_ex failed");
		}
	}

	const EVP_MD* SslDigest::getEvpMd(SslDigest::Type digestType) {
		return digestMds[static_cast<std::size_t>(digestType)]();
	}

	std::string SslDigest::getHexString(unsigned char* data, unsigned int numElements) {
		std::stringstream target;
		for (std::size_t i{ 0 }; i < numElements; i++) {
			target << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(data[i]);
		}
		return target.str();
	}

	const EVP_MD* SslDigest::getMd5() {
		static OSSL_PROVIDER* legacy = OSSL_PROVIDER_load(NULL, "legacy");
		static OSSL_PROVIDER* default_provider = OSSL_PROVIDER_load(NULL, "default");
		return EVP_md5();
	}

	void swap(SslDigest& lhs, SslDigest& rhs) noexcept {
		std::swap(lhs.g_mdctx, rhs.g_mdctx);
	}

} // namespace Updater2::IO