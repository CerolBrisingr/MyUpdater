find_package(OpenSSL REQUIRED)
find_package(unofficial-bit7z CONFIG REQUIRED)

add_library(Fileinteractions STATIC)
target_sources(Fileinteractions
    PUBLIC
        src/fileinteractions.cpp
        src/ssldigest.cpp
)

if (WIN32)
target_sources(Fileinteractions
    PUBLIC
        src/fileinteractions_windows.cpp
    )
endif()

target_compile_definitions(Fileinteractions 
    PRIVATE 
        BIT7Z_AUTO_FORMAT
)

target_include_directories(Fileinteractions
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(Fileinteractions
    PUBLIC
        OpenSSL::Crypto
        unofficial::bit7z::bit7z64)
        
if (BUILD_TESTING)
    add_subdirectory(tests)
endif()