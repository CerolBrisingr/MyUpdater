#include "IO/ssldigest.h"

namespace Updater2::IO {

	SslDigest::SslDigest(SslDigest::Type digestType) {
		const EVP_MD* md{ EVP_get_digestbyname(getEvpMdName(digestType).c_str()) };
		if (md == nullptr) return;

		mdctx = EVP_MD_CTX_new();
		if (mdctx == nullptr) return;

		EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);
	}

	SslDigest::~SslDigest() {
		EVP_MD_CTX_free(mdctx);
	}

	SslDigest::SslDigest(SslDigest&& rhs) noexcept
		:mdctx{ rhs.mdctx }
	{
		rhs.mdctx = nullptr;
	}

	SslDigest& SslDigest::operator= (SslDigest&& rhs) noexcept
	{
		if (&rhs == this) { return *this; }
		EVP_MD_CTX_free(mdctx);
		this->mdctx = rhs.mdctx;
		rhs.mdctx = nullptr;
		return *this;
	}

	void SslDigest::update(const void* data, std::size_t count) {
		if (isValid()) {
			EVP_DigestUpdate(mdctx, data, count);
		}
	}

	bool SslDigest::isValid() const {
		return mdctx != nullptr;
	}

	std::string SslDigest::finalize() {
		if (!isValid()) return "";
		unsigned char md_value[EVP_MAX_MD_SIZE];
		unsigned int md_len;
		EVP_DigestFinal_ex(mdctx, md_value, &md_len);
		return getHexString(md_value, md_len);
	}

	std::string SslDigest::getEvpMdName(SslDigest::Type digestType) {
		assert(digestType != Type::numTypes && "Chosen digestType is reserved for internal use");
		return std::string{ digestNames[static_cast<std::size_t>(digestType)] };
	}

	std::string SslDigest::getHexString(unsigned char* data, unsigned int numElements) {
		std::stringstream target;
		for (int i{ 0 }; i < numElements; i++) {
			target << std::hex << std::setfill('0') << std::setw(2) << static_cast<int>(data[i]);
		}
		return target.str();
	}

} // namespace Updater2::IO