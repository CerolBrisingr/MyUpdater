#include <io/fileinteractions.h>

#include <iostream>

#include <gtest/gtest.h>

namespace ssl = Updater2::IO;

TEST(FilesystemTest, Bonus) {
	EXPECT_TRUE(nullptr == NULL);
}

// Demonstrate some basic assertions.
TEST(FilesystemTest, BasicAssertions) {
	EXPECT_TRUE(ssl::isFolder("../md5"));
	EXPECT_TRUE(ssl::isFile("md5_result.txt"));
}

TEST(FilesystemTest, Md5) {
	auto md5Calc{ ssl::calculateMd5Hash("md5_target.txt") };
	EXPECT_FALSE(ssl::compareMd5Hashes(md5Calc, ""));
	auto md5Calc2{ ssl::calculateMd5Hash("md5_target2.txt") };
	EXPECT_FALSE(ssl::compareMd5Hashes(md5Calc2, md5Calc));
	auto md5Read{ ssl::readFirstLineInFile("md5_result.txt") };
	EXPECT_FALSE(ssl::compareMd5Hashes(md5Read, ""));
	EXPECT_TRUE(ssl::compareMd5Hashes(md5Calc, md5Read));
}
