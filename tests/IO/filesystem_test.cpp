#include "Updater2/IO/filesystem.h"

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
}

TEST(FilesystemTest, FolderContent) {
	EXPECT_TRUE(myfs::isFolder("../tests"));
}