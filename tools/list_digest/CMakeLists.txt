option(Updater2_SSL_BUILD_DIGEST_LIST "Build digest name explorer" ON)
if (Updater2_SSL_BUILD_DIGEST_LIST)
	find_package(OpenSSL REQUIRED)

    add_executable(SSLDigestNameLister)
    target_sources(SSLDigestNameLister
        PRIVATE
            list_digest.cpp)
    target_link_libraries(SSLDigestNameLister
        PRIVATE
            OpenSSL::Crypto)
endif()