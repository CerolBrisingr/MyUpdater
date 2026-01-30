set(X_VCPKG_APPLOCAL_COPY_INSTALL ON CACHE BOOL "Kopiert sekundäre DLLs automatisch beim Installieren")
cmake_minimum_required(VERSION 3.13) # CMP0079, allow target_link_libraries to access other directories
cmake_policy(SET CMP0079 NEW)
project(CustomUpdater VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(Updater2_BUILD_TESTS "Build unit tests" ON)
option(Updater2_USE_QT_UI "Use QT with widgets" OFF)

if(Updater2_USE_QT_UI)
    find_package(Qt6 REQUIRED COMPONENTS Widgets)
else()
    find_package(Qt6 REQUIRED COMPONENTS Core)
endif()

if(Updater2_BUILD_TESTS)
    find_package(GTest CONFIG REQUIRED)
    include(CTest)
endif()

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install" CACHE PATH "Install path" FORCE)
add_subdirectory(libs/io)
add_subdirectory(tools/list_digest)
add_subdirectory(app)

if(Updater2_USE_QT_UI)
    target_link_libraries(Updater2 PRIVATE Qt6::Widgets)

    set_target_properties(Updater2 PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
    )
endif()

if(BUILD_TESTING)
    #add_subdirectory(app/tests) #Does not exist yet
endif()
