#include "io/ssldigest.h"

#include <gtest/gtest.h>
#include <string_view>
#include <cstring>
#include <cstddef>

using namespace std::literals;

constexpr const char* md5_result{ "1ad2ec548147a57a706fac2e1ec7112c" };
constexpr const char* md5_input{ "Do not change content!" };
const std::size_t md5_input_size{ std::strlen(md5_input) };

namespace myfs = Updater2::IO;

TEST(SslDigestTest, GetMd5){
	myfs::SslDigest digest{ myfs::SslDigest::Type::MD5 };
	EXPECT_TRUE(true);
	digest.update(md5_input, md5_input_size);
	EXPECT_STREQ(md5_result, digest.finalize().c_str());
}