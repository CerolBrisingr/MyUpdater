#include "io/ssldigest.h"

#include <gtest/gtest.h>
#include <string_view>
#include <cstring>
#include <cstddef>

using namespace std::literals;

constexpr std::string_view md5_result{ "1ad2ec548147a57a706fac2e1ec7112c" };
constexpr std::string_view md5_empty{ "d41d8cd98f00b204e9800998ecf8427e" };
constexpr const char* md5_input{ "Do not change content!" };
constexpr std::size_t md5_input_size{ std::char_traits<char>::length(md5_input) };
constexpr std::size_t bundle_size{ 10 };
static_assert(md5_input_size > bundle_size, "Source string must be longer than bundle size");

namespace myfs = Updater2::IO;

struct Snippet {
	const char* text{ nullptr };
	std::size_t length{ 0 };
};

constexpr Snippet firstSet{
	.text{md5_input},
	.length{bundle_size}
};

constexpr Snippet secondSet{
	.text{md5_input + bundle_size},
	.length{md5_input_size - bundle_size}
};

TEST(SslDigestTest, GetMd5) {
	myfs::SslDigest digest{ myfs::SslDigest::Type::MD5 };
	digest.update(md5_input, md5_input_size);
	EXPECT_EQ(md5_result, digest.finalize());
	EXPECT_EQ(md5_empty, digest.finalize());  // Expect context to be re-initalized
	digest.update(md5_input, md5_input_size);
	EXPECT_EQ(md5_result, digest.finalize());
}

TEST(SslDigestTest, Copy) {
	myfs::SslDigest digest{ myfs::SslDigest::Type::MD5 };
	digest.update(firstSet.text, firstSet.length);
	myfs::SslDigest digest2{ digest };
	EXPECT_EQ(digest2.finalize(), digest.finalize());
}

TEST(SslDigestTest, Copy2) {
	myfs::SslDigest digest{ myfs::SslDigest::Type::MD5 };
	digest.update(firstSet.text, firstSet.length);
	myfs::SslDigest digest2{ digest };
	myfs::SslDigest digest3{ myfs::SslDigest::Type::SHA2_384 };
	digest3 = digest;
	digest.update(secondSet.text, secondSet.length);
	std::string md5{ digest.finalize() };
	EXPECT_NE(digest2.finalize(), md5);
	EXPECT_NE(digest3.finalize(), md5);
}

TEST(SslDigestTest, Copy3) {
	myfs::SslDigest digest{ myfs::SslDigest::Type::MD5 };
	digest.update(firstSet.text, firstSet.length);
	myfs::SslDigest digest2{ digest };
	myfs::SslDigest digest3{ myfs::SslDigest::Type::SHA2_384 };
	digest3 = digest;
	digest.update(secondSet.text, secondSet.length);
	digest2.update(secondSet.text, secondSet.length);
	digest3.update(secondSet.text, secondSet.length);
	std::string md5{ digest.finalize() };
	EXPECT_EQ(md5_result, md5);
	EXPECT_EQ(digest2.finalize(), md5);
	EXPECT_EQ(digest3.finalize(), md5);
}