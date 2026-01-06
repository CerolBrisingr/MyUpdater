#include <IO/fileinteractions.h>

#include <cstdlib>
#include <iostream>

#include <gtest/gtest.h>

namespace myfs = Updater2::IO;

// Demonstrate some basic assertions.
TEST(FilesystemTest, BasicAssertions) {
	// Expect two strings not to be equal.
	EXPECT_STRNE("hello", "world");
	// Expect equality.
	EXPECT_EQ(7 * 6, 42);
	// Access tested file
	myfs::printCurrentPath();
	
	if (const char* env_t = std::getenv("TEST_DATA_DIR"))
		std::cout << "Your environment variable contains: " << env_t << '\n';
	else
		std::cout << "Environment variable not found!\n";
}

TEST(FilesystemTest, FindFolder) {
	EXPECT_TRUE(myfs::isFolder("../md5"));
}

TEST(FilesystemTest, FindFile) {
	EXPECT_TRUE(myfs::isFile("md5_result.txt"));
}