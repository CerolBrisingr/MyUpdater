include(GoogleTest)
find_package(OpenSSL REQUIRED)

block(PROPAGATE
        SSL_ASSET_CA_CERT
        SSL_ASSET_SERVER_KEY
        SSL_ASSET_SERVER_CERT
        SSL_ASSET_SERVER_TIMEOUT_KEY
        SSL_ASSET_SERVER_TIMEOUT_CERT
        SSL_ASSET_SERVER_NO_CA_KEY
        SSL_ASSET_SERVER_NO_CA_CERT
        )
    include(buildCertificates.cmake)
endblock()

add_executable(
        downloader_test
        downloader_test.cpp
)

target_link_libraries( downloader_test
        PRIVATE
            Downloader
            OpenSSL::Crypto
            OpenSSL::SSL
            GTest::gtest_main
)

target_compile_features(downloader_test
        PUBLIC
            cxx_std_20
)

target_compile_definitions(downloader_test
    PRIVATE
        SSL_ASSET_CA_CERT
        SSL_ASSET_SERVER_KEY
        SSL_ASSET_SERVER_CERT
        SSL_ASSET_SERVER_TIMEOUT_KEY
        SSL_ASSET_SERVER_TIMEOUT_CERT
        SSL_ASSET_SERVER_NO_CA_KEY
        SSL_ASSET_SERVER_NO_CA_CERT
)

target_include_directories(
        downloader_test
            PUBLIC
                "${CMAKE_CURRENT_LIST_DIR}/external/"
)

gtest_discover_tests(downloader_test)
