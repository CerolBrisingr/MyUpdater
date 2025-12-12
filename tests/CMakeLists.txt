add_executable(
  filesystem_test
  IO/filesystem_test.cpp
)

target_link_libraries( filesystem_test
	PRIVATE
		Updater2_Core
		GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(filesystem_test)