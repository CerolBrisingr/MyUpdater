add_executable(
  ssldigest_test
  ssldigest/ssldigest_test.cpp
)

target_link_libraries( ssldigest_test
	PRIVATE
		Fileinteractions
		GTest::gtest_main
)

add_executable(
  fileinteraction_test
  fileinteraction/fileinteraction_test.cpp
)

target_link_libraries( fileinteraction_test
	PRIVATE
		Fileinteractions
		GTest::gtest_main
)

include(GoogleTest)

set(ASSET_DIR ${CMAKE_CURRENT_LIST_DIR}/../test_assets)
message("Asset_dir " ${ASSET_DIR})
gtest_discover_tests(ssldigest_test)
gtest_discover_tests(fileinteraction_test
	PROPERTIES
		WORKING_DIRECTORY ${ASSET_DIR}/md5
)