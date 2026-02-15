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

# Find 7zip library
if(WIN32 AND VCPKG_TOOLCHAIN)
    find_file(LIB_7Z_PATH
            NAMES 7z.dll 7zip.dll
            PATHS "${_VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/bin"
            NO_DEFAULT_PATH
            REQUIRED
    )
    # TODO: Extract pure filename (e.g., 7z.dll) for runtime loading
    # get_filename_component(7Z_LIB_FILENAME "${7Z_DLL_PATH}" NAME)
    # Move dll file to executable (custom_command)
    # Prepare for install
    # Put that repetitive stuff in a function
else()
    block(PROPAGATE LIB_7Z_PATH)
        set(CMAKE_FIND_LIBRARY_PREFIXES "${CMAKE_FIND_LIBRARY_PREFIXES};")
        find_library(LIB_7Z_PATH NAMES 7z PATHS "/usr/lib/7zip" "/usr/local/lib/7zip" REQUIRED)
    endblock()
endif()

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
else()
    target_sources(Fileinteractions
        PUBLIC
            src/fileinteractions_linux.cpp
    )
endif()

message(STATUS "7-Zip Binary used: ${LIB_7Z_PATH}")

target_compile_definitions(Fileinteractions 
    PUBLIC
        BIT7Z_AUTO_FORMAT
        LIB_7Z_SHARED_LIBRARY_PATH="${LIB_7Z_PATH}"
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
