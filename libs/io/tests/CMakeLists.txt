include(GoogleTest)

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

gtest_discover_tests(ssldigest_test)
gtest_discover_tests(fileinteraction_test)