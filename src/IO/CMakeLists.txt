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
        