option(FILEINTERACTIONS_STRICT_WARNINGS "Build with strict warnings" ON)

add_library(fileinteractions_strict_project_warnings INTERFACE)
target_compile_options(fileinteractions_strict_project_warnings INTERFACE
    $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
    $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall -Wextra -Werror>
)

find_package(OpenSSL REQUIRED)

# Build bit7z
set(BIT7Z_AUTO_FORMAT ON CACHE INTERNAL "" FORCE)
set(BIT7Z_DETECT_FROM_EXTENSION OFF CACHE INTERNAL "" FORCE)
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/external/bit7z)

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
        bit7z)

if(FILEINTERACTIONS_STRICT_WARNINGS)
    target_link_libraries(Fileinteractions
        PRIVATE
            fileinteractions_strict_project_warnings
    )
endif()
        
if (BUILD_TESTING)
    add_subdirectory(tests)
endif()
