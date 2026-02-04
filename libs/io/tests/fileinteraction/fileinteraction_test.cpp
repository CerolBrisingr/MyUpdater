#include <io/fileinteractions.h>

#include <iostream>
#include <exception>
#include <format>
#include <string_view>
#include <system_error>
#include <bit7z/bitarchivewriter.hpp>
#include <tuple>
#include <thread>

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

namespace Zip {

	constexpr std::string_view g_name1{ "myArchiveFile.txt" };
	constexpr std::string_view g_name2{ "myArchiveFile2.txt" };
	constexpr std::string_view g_archiveName{ "myTestArchive.7z" };
	constexpr std::string_view g_extractName{ "unzipFolder" };

	namespace {
		void buildArchive() {
			// Not catching anything here. If it throws, the test fails.
			bit7z::Bit7zLibrary lib(BIT7Z_STRING("7zip.dll"));
			bit7z::BitArchiveWriter archive{ lib, bit7z::BitFormat::SevenZip };
			archive.addFile(g_name1.data());
			archive.addFile(g_name2.data());
			archive.compressTo(g_archiveName.data());
		}
	} // namespace

	class ArchiveTest : public ::testing::Test {
	protected:
		void SetUp() override {
			TearDown();
		}

		void TearDown() override {
			static constexpr std::array files{ g_name1 , g_name2, g_archiveName };
			std::error_code ec;
			for (std::string_view filename : files) {
				myfs::removeFileNoThrow(filename, ec);
			}
		}
	};

	TEST_F(ArchiveTest, BuildZip) {
		myfs::writeStringAsFile(g_name1, "this is a text!");
		myfs::writeStringAsFile(g_name2, "this is another text!");
		buildArchive();

		auto info = myfs::inspectArchive(g_archiveName.data());
		ASSERT_TRUE(info) << "inspectArchive failed to return Information!";
		ASSERT_EQ(info->filesCount, 2);
		ASSERT_EQ(info->foldersCount, 0);
		
		myfs::unzipArchive(g_archiveName, g_extractName);
		ASSERT_TRUE(myfs::isFolder(g_extractName)) << "Missing unzip target Folder: " << g_extractName;
		ASSERT_TRUE(myfs::isFile(std::format("{}/{}", g_extractName, g_name1))) << "Missing unzip target file: " << g_name1;
		ASSERT_TRUE(myfs::isFile(std::format("{}/{}", g_extractName, g_name2))) << "Missing unzip target file: " << g_name2;

		ASSERT_EQ(3, myfs::removeFolderRecursively(g_extractName)) << "Cleanup count mismatch! Expecting 1 Folder and 2 Files in: " << g_extractName;
	}

} // namespace zip

namespace Executable {

	namespace {
		bool waitForFile(std::string_view fileName) {
			for (int i = 0; i < 50; ++i) { // Max 5 Sekunden (50 * 100ms)
				if (std::filesystem::exists(fileName) && std::filesystem::file_size(fileName) > 0) {
					std::this_thread::sleep_for(std::chrono::milliseconds(100)); // reduce risk of file being still worked on
					return true;
				}
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
			return false; // File never showed up
		}
	}

	class ExecutableStarterTest 
		:public ::testing::TestWithParam<std::tuple<std::wstring, std::string>> {
	};

	using TestParam = std::tuple<std::wstring, std::string> ;

	INSTANTIATE_TEST_SUITE_P(
		ArgumentGroup, ExecutableStarterTest,
		testing::Values(
			TestParam{ L"-verify_this", "-verify_this"},
			TestParam{ L"\"quoted argument\"", "quoted argument"}
		)
	);

	TEST_P(ExecutableStarterTest, commandline) {
		ASSERT_TRUE(myfs::removeFileNoThrow("output.txt")) << "Previous output still exists and could not be removed!";
		const auto& [arguments, verification] = GetParam();
		bool result = Updater2::IO::createProcess("./commandline_printer.exe", arguments);
		ASSERT_TRUE(result) << "Failed to run target executable";
		ASSERT_TRUE(waitForFile("output.txt"));
		std::string lineString{ myfs::readFirstLineInFile("output.txt") };
		EXPECT_EQ(lineString, std::format("./commandline_printer.exe {}", verification));
		myfs::removeFileNoThrow("output.txt");
	}

} // namespace Executable