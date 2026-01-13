find_package(OpenSSL REQUIRED)

add_library(Fileinteractions STATIC)
target_sources(Fileinteractions
    PUBLIC
        src/fileinteractions.cpp 
        src/ssldigest.cpp)

target_include_directories(Fileinteractions
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include)

target_link_libraries(Fileinteractions
    PUBLIC
        OpenSSL::Crypto)
        