#include <io/fileinteractions.h>

#include <iostream>
#include <exception>
#include <string_view>
#include <system_error>
#include <array>

#include <gtest/gtest.h>

namespace myfs = Updater2::IO;

namespace {
	constexpr std::string_view g_md5_source_content{ "Do not change content!" };
	constexpr std::string_view g_md5_source_name{ "md5_target.txt" };
	constexpr std::string_view g_md5_source_hash{ "1ad2ec548147a57a706fac2e1ec7112c" };
	constexpr std::string_view g_md5_mismatch_content {"Do not change content either!" };
	constexpr std::string_view g_md5_mismatch_name{ "md5_target2.txt" };

	static_assert(g_md5_source_hash.size() == 32, "MD5 hash must be exactly 32 characters!");
} // namespace


namespace MD5 {

	class Md5Files : public ::testing::Test {
	protected:
		void SetUp() override {
			try {
				myfs::writeStringAsFile(g_md5_source_name, g_md5_source_content);
				myfs::writeStringAsFile(g_md5_mismatch_name, g_md5_mismatch_content);
			}
			catch (const std::exception& ex) {
				GTEST_SKIP() << "Test setup failed due to exception: \"" << ex.what() << '\"';
			}
			catch (const std::string& ex) {
				GTEST_SKIP() << "Test setup failed due to exception: \"" << ex << '\"';
			}
			catch (...) {
				GTEST_SKIP() << "Test setup failed due to unknown exception";
			}
		}

		void TearDown() override {
			static constexpr std::array files{g_md5_source_name , g_md5_mismatch_name};
			std::error_code ec;
			for (std::string_view filename : files) {
				myfs::removeFileNoThrow(filename, ec);
			}
		}
	};

	TEST_F(Md5Files, FileOperations) {
		auto md5Calc{ myfs::calculateMd5HashFromFile(g_md5_source_name.data()) };
		EXPECT_FALSE(myfs::compareMd5Hashes(md5Calc, ""));
		EXPECT_NE(md5Calc, "");
		auto md5Calc2{ myfs::calculateMd5HashFromFile(g_md5_mismatch_name.data()) };
		EXPECT_FALSE(myfs::compareMd5Hashes(md5Calc2, md5Calc));
		EXPECT_NE(md5Calc2, md5Calc);
		EXPECT_EQ(g_md5_source_hash, md5Calc);
		EXPECT_TRUE(myfs::compareMd5Hashes(g_md5_source_hash, md5Calc));
	}

} // namespace MD5

namespace zip {

	TEST(Unzip, ManualPrep) {
		EXPECT_TRUE(myfs::unzipArchive("testArchive.7z", "outFolder"));
	}

} // namespace zip