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

gtest_discover_tests(ssldigest_test)


add_executable(
  fileinteraction_test
  fileinteraction/fileinteraction_test.cpp
)

target_link_libraries( fileinteraction_test
	PRIVATE
		Fileinteractions
		GTest::gtest_main
)

target_compile_definitions(Fileinteractions 
    PRIVATE 
        BIT7Z_AUTO_FORMAT
)

if(WIN32)
    add_custom_command(TARGET fileinteraction_test POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:fileinteraction_test>
        $<TARGET_FILE_DIR:fileinteraction_test>
        COMMAND_EXPAND_LISTS
    )
endif()

gtest_discover_tests(fileinteraction_test)