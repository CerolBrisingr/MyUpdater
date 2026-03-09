find_package(CURL REQUIRED)

add_library(Downloader INTERFACE)

# TODO: Apply to all targets intended to be "modular"
target_compile_features(Downloader
    INTERFACE
        cxx_std_20
)

target_include_directories(Downloader
    INTERFACE
        ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(Downloader
    INTERFACE
        CURL::libcurl
)
        
if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
