find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

# add_library(Downloader STATIC)
add_executable(Downloader)
target_sources(Downloader
    PUBLIC
        src/provisional_runner.cpp)

target_include_directories(Downloader
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(Downloader
    PUBLIC
        CURL::libcurl
        OpenSSL::SSL)
        
#[[if (BUILD_TESTING)
    add_subdirectory(tests)
endif() #]]

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