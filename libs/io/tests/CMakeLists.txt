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

target_compile_definitions(fileinteraction_test 
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

add_executable( commandline_printer
    fileinteraction/commandline_printer.cpp
)

add_dependencies(fileinteraction_test commandline_printer)

set(PRINTER_FILE "output.txt")

target_compile_definitions(fileinteraction_test
    PRIVATE
        COMMANDLINE_PRINTER="$<TARGET_FILE:commandline_printer>"
        COMMANDLINE_PRINTER_FILE="${PRINTER_FILE}"
        LIB_7Z_SHARED_LIBRARY_PATH="${LIB_7Z_PATH}"
)

target_compile_definitions(commandline_printer
    PRIVATE
        COMMANDLINE_PRINTER_FILE="${PRINTER_FILE}"
)

if(FILEINTERACTIONS_STRICT_WARNINGS)
    target_link_libraries(fileinteraction_test
        PRIVATE
            fileinteractions_strict_project_warnings
    )
    target_link_libraries(commandline_printer
        PRIVATE
            fileinteractions_strict_project_warnings
    )
endif()

gtest_discover_tests(fileinteraction_test)