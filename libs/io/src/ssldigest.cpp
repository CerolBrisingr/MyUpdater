#include "IO/ssldigest.h"

namespace Updater2::IO {

	SslDigest::SslDigest(Type digestType)
		:g_digestType{digestType}
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
		:g_digestType{rhs.g_digestType }
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
		:g_mdctx{ std::move(rhs.g_mdctx) }, g_digestType{rhs.g_digestType }
	{}

	SslDigest& SslDigest::operator= (SslDigest&& rhs) noexcept
	{
		if (&rhs == this) { return *this; }
		g_mdctx.swap(rhs.g_mdctx);
		std::swap(g_digestType, rhs.g_digestType);
		return *this;
	}

	void SslDigest::update(const void* data, std::size_t count) {
		if (EVP_DigestUpdate(g_mdctx.get(), data, count) != 1) {
			throw std::runtime_error("EVP_DigestUpdate failed");
		}
	}

	std::string SslDigest::finalize() {
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
		const EVP_MD* digestAlgorithm{ getEvpMd(g_digestType) };
		if (!digestAlgorithm) throw std::logic_error("Requested digest is not available");
		if (EVP_DigestInit_ex(g_mdctx.get(), digestAlgorithm, nullptr) != 1)
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

	void swap(SslDigest& lhs, SslDigest& rhs) noexcept {
		std::swap(lhs.g_digestType, rhs.g_digestType);
		std::swap(lhs.g_mdctx, rhs.g_mdctx);
	}

} // namespace Updater2::IO