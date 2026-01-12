find_package(OpenSSL REQUIRED)

add_library(Fileinteractions STATIC)
target_sources(Fileinteractions
    PUBLIC
        fileinteractions.cpp)

target_include_directories(Fileinteractions
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include)

target_link_libraries(Fileinteractions
    PUBLIC
        OpenSSL::Crypto)

option(Updater2_SSL_BUILD_DIGEST_LIST "Build digest name explorer" ON)
if (Updater2_SSL_BUILD_DIGEST_LIST)
    add_executable(SSLDigestNameLister)
    target_sources(SSLDigestNameLister
        PRIVATE
            list_digest.cpp)
    target_link_libraries(SSLDigestNameLister
        PRIVATE
            OpenSSL::Crypto)
endif()
        