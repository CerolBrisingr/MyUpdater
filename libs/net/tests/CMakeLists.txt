include(GoogleTest)
find_package(OpenSSL REQUIRED)

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

target_include_directories(
        downloader_test
            PUBLIC
                "${CMAKE_CURRENT_LIST_DIR}/external/"
)

gtest_discover_tests(downloader_test)