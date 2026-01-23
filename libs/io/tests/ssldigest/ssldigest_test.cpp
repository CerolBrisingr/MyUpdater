#include "io/ssldigest.h"

#include <gtest/gtest.h>
#include <string_view>
#include <cstring>
#include <cstddef>

constexpr std::string_view md5_result{ "1ad2ec548147a57a706fac2e1ec7112c" };
constexpr std::string_view md5_empty{ "d41d8cd98f00b204e9800998ecf8427e" };
constexpr const char* md5_input{ "Do not change content!" };
constexpr std::size_t md5_input_size{ std::char_traits<char>::length(md5_input) };
constexpr std::size_t bundle_size{ 10 };
static_assert(md5_input_size > bundle_size, "Source string must be longer than bundle size");

namespace ssl = Updater2::SSL;

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
	ssl::SslDigest digest{ ssl::SslDigest::Type::MD5 };
	digest.update(md5_input, md5_input_size);
	EXPECT_EQ(md5_result, digest.finalize());
	EXPECT_EQ(md5_empty, digest.finalize());  // Expect context to be re-initalized
	digest.update(md5_input, md5_input_size);
	EXPECT_EQ(md5_result, digest.finalize());
}

TEST(SslDigestTest, Copy) {
	ssl::SslDigest digest{ ssl::SslDigest::Type::MD5 };
	digest.update(firstSet.text, firstSet.length);
	ssl::SslDigest digest2{ digest };
	EXPECT_EQ(digest2.finalize(), digest.finalize());
}

TEST(SslDigestTest, Copy2) {
	ssl::SslDigest digest{ ssl::SslDigest::Type::MD5 };
	digest.update(firstSet.text, firstSet.length);
	ssl::SslDigest digest2{ digest };
	ssl::SslDigest digest3{ ssl::SslDigest::Type::SHA2_384 };
	digest3 = digest;
	digest.update(secondSet.text, secondSet.length);
	std::string md5{ digest.finalize() };
	EXPECT_NE(digest2.finalize(), md5);
	EXPECT_NE(digest3.finalize(), md5);
}

TEST(SslDigestTest, Copy3) {
	ssl::SslDigest digest{ ssl::SslDigest::Type::MD5 };
	digest.update(firstSet.text, firstSet.length);
	ssl::SslDigest digest2{ digest };
	ssl::SslDigest digest3{ ssl::SslDigest::Type::SHA2_384 };
	digest3 = digest;
	digest.update(secondSet.text, secondSet.length);
	digest2.update(secondSet.text, secondSet.length);
	digest3.update(secondSet.text, secondSet.length);
	std::string md5{ digest.finalize() };
	EXPECT_EQ(md5_result, md5);
	EXPECT_EQ(digest2.finalize(), md5);
	EXPECT_EQ(digest3.finalize(), md5);
}

TEST(SslDigestTest, Move) {
	ssl::SslDigest source{ ssl::SslDigest::Type::MD5 };
	source.update(md5_input, std::strlen(md5_input));
	ssl::SslDigest target{ std::move(source) };
	EXPECT_THROW(source.update("test", 4), std::runtime_error);
	EXPECT_EQ(md5_result, target.finalize());
	source = ssl::SslDigest{ ssl::SslDigest::Type::MD5 };  // Reuse
	EXPECT_EQ(md5_empty, source.finalize());
}

testing::AssertionResult singleUse_isInvalidOrSwapped(const char* expr, ssl::SslDigest& source) {
	// Assumptions being made:
	// 1) source is left over after >> target = std::move(source);
	// 2) Initial target state is newly initialized and using md5
	try {
		source.update(md5_input, md5_input_size);
		std::string output{ source.finalize() };
		if (output == md5_result) {
			// Not invalidated but swapped with target state
			return testing::AssertionSuccess();
		}
		return testing::AssertionFailure()
			<< "Expression: " << expr
			<< "\n  Expected hash: " << md5_result
			<< "\n  Actual hash:   " << output;
	}
	catch (const std::runtime_error& e) {
		if (strcmp(e.what(), ssl::move_on_error)) {
			// Success case: It was correctly invalidated
			return testing::AssertionSuccess();
		}
		else {
			// Runtime_exception as expected but the message does not match. Something else failed.
			return testing::AssertionFailure() << "Unexpected message in runtime-exception thrown from " << expr
				<< "\nMessage: " << e.what();
		}
	}
	catch (...) {
		return testing::AssertionFailure() << "Unexpected exception type thrown from " << expr;
	}
}

TEST(SslDigestTest, Move2) {
	ssl::SslDigest source{ ssl::SslDigest::Type::MD5 };
	source.update(md5_input, std::strlen(md5_input));
	ssl::SslDigest target{ ssl::SslDigest::Type::MD5 };
	target = std::move(source);
	EXPECT_EQ(md5_result, target.finalize());
	EXPECT_PRED_FORMAT1(singleUse_isInvalidOrSwapped, source);
}