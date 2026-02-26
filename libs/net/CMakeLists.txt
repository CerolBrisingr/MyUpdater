find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

add_library(Downloader INTERFACE)
#[[add_executable(Downloader)
target_sources(Downloader
    PUBLIC
        src/provisional_runner.cpp)
#]]
# TODO: Remove associated files before removing this comment

# TODO: Apply to all targets intended to be "modular"
target_compile_features(Downloader
    INTERFACE
        cxx_std_20
)

target_include_directories(Downloader
    INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/include)

# TODO: Test/find out if OpenSSL::SSL is actively needed or implicitly available
target_link_libraries(Downloader
    INTERFACE
        CURL::libcurl
        OpenSSL::SSL)
        
if (BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(CURL_HTTPS_FOUND)
  message("CURL supports HTTPS")
else()
  message("CURL is missing HTTPS access")
endif()

if(CURL_SSL_FOUND)
  message("CURL supports SSL")
else()
  message("CURL is missing SSL")
endif()