include(GoogleTest)
find_package(OpenSSL REQUIRED)
find_program(OPENSSL_EXECUTABLE openssl REQUIRED)

add_executable(
        downloader_test
        certificate_handler.cpp
        downloader_test.cpp
)

target_link_libraries( downloader_test
        PRIVATE
            Downloader
            Fileinteractions
            OpenSSL::Crypto
            OpenSSL::SSL
            GTest::gtest_main
)

target_compile_features(downloader_test
        PUBLIC
            cxx_std_20
)

set(CA_CONFIG_PATH "${CMAKE_CURRENT_LIST_DIR}/certs/ca.conf")
set(SERVER_CONFIG_PATH "${CMAKE_CURRENT_LIST_DIR}/certs/server.conf")

target_compile_definitions(downloader_test
    PRIVATE
        OPEN_SSL="${OPENSSL_EXECUTABLE}"
        CA_CONFIG="${CA_CONFIG_PATH}"
        SERVER_CONFIG="${SERVER_CONFIG_PATH}"
        UNIT_TESTING
)

target_include_directories(
        downloader_test
            PUBLIC
                "${CMAKE_CURRENT_LIST_DIR}/external/"
                "${CMAKE_CURRENT_LIST_DIR}/include/"
)

gtest_discover_tests(downloader_test)
