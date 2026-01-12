#include <IO/fileinteractions.h>

#include <iostream>

#include <gtest/gtest.h>

namespace myfs = Updater2::IO;

TEST(FilesystemTest, Bonus) {
	EXPECT_TRUE(nullptr == NULL);
}

// Demonstrate some basic assertions.
TEST(FilesystemTest, BasicAssertions) {
	EXPECT_TRUE(myfs::isFolder("../md5"));
	EXPECT_TRUE(myfs::isFile("md5_result.txt"));
}

TEST(FilesystemTest, Md5) {
	auto md5Calc{ myfs::calculateMd5Hash("md5_target.txt") };
	EXPECT_FALSE(myfs::compareMd5Hashes(md5Calc, ""));
	auto md5Read{ myfs::readFirstLineInFile("md5_result.txt") };
	EXPECT_FALSE(myfs::compareMd5Hashes(md5Read, ""));
	EXPECT_TRUE(myfs::compareMd5Hashes(md5Calc, md5Read));
}
