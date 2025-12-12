cmake_minimum_required(VERSION 3.13) # CMP0079
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

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_BINARY_DIR}/install" CACHE PATH "Install path" FORCE)
add_subdirectory(src)

if(Updater2_USE_QT_UI)
    target_link_libraries(Updater2 PRIVATE Qt6::Widgets)

    set_target_properties(Updater2 PROPERTIES
        WIN32_EXECUTABLE ON
        MACOSX_BUNDLE ON
    )
endif()

if(Updater2_BUILD_TESTS)
    include(FetchContent)
    FetchContent_Declare(
      googletest
      URL https://github.com/google/googletest/archive/52eb8108c5bdec04579160ae17225d66034bd723.zip
    )
    # For Windows: Prevent overriding the parent project's compiler/linker settings
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(googletest)
    
    include(CTest)
    enable_testing()
    add_subdirectory(tests)
endif()
