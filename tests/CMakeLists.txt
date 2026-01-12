add_executable(
  fileinteraction_test
  IO/fileinteraction_test.cpp
)

target_link_libraries( fileinteraction_test
	PRIVATE
		Fileinteractions
		GTest::gtest_main
)

include(GoogleTest)

set(ASSET_DIR ${PROJECT_SOURCE_DIR}/assets)
gtest_discover_tests(fileinteraction_test
	PROPERTIES
		WORKING_DIRECTORY ${ASSET_DIR}/md5
)