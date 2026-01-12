#include "IO/fileinteractions.h"

namespace fs = std::filesystem;

namespace Updater2::IO {

	SslDigest::SslDigest(SslDigest::Type digestType) {
		const EVP_MD* md { EVP_get_digestbyname(getEvpMdName(digestType).c_str()) };
		if (md == nullptr) return;

		mdctx = EVP_MD_CTX_new();
		if (mdctx == nullptr) return;

		EVP_DigestInit_ex(mdctx, EVP_md5(), NULL);
	}

	SslDigest::~SslDigest() {
		EVP_MD_CTX_free(mdctx);
	}

	SslDigest::SslDigest(SslDigest&& rhs) noexcept
		:mdctx{rhs.mdctx}
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
		return std::string{ reinterpret_cast<char*>(md_value), md_len };
	}

	std::string SslDigest::getEvpMdName(SslDigest::Type digestType) {
		assert(digestType != Type::numTypes && "Chosen digestType is reserved for internal use");
		return std::string{ digestNames[static_cast<std::size_t>(digestType)] };
	}

	constexpr std::size_t blockSize{ 1024 * 1024 };

	std::string calculateMd5Hash(const std::string& filename)
	{
		std::cout << "Will calculate md5 hash for file: " << filename << '\n';
		SslDigest digest{ SslDigest::Type::MD5 };
		std::ifstream file(filename, std::ios::binary | std::ios::ate);
		if (file.is_open()) {
			auto size = file.tellg();
			std::cout << "Size of file: " << size << '\n';
		}
		return digest.finalize();
	}

	bool compareMd5Hashes(std::string_view hash1, std::string_view hash2)
	{
		return hash1.compare( hash2 ) == 0;
	}

	void printCurrentPath()
	{
		std::cout << "Current path is: \"" << fs::current_path() << "\"\n";
	}

	bool isFolder(std::string_view path_in)
	{
		return fs::is_directory(path_in);
	}

	bool isFile(std::string_view path_in)
	{
		return fs::is_regular_file(path_in);
	}

	bool writeStringAsFile(const std::string &filename, std::string_view filecontent)
	{
		std::ofstream target( filename, std::ios::out);
		if (target) {
			target << filecontent;
		}
		else {
			return false;
		}
		return true;
	}

	std::string readFirstLineInFile(const std::string& filename) {
		std::ifstream target(filename, std::ios::in);
		return "Not ready!";
	}

} // namespace Updater2::IO