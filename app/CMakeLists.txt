
add_library(Updater2_Core)
target_sources(Updater2_Core
    PUBLIC
        src/hello.cpp)

target_include_directories(Updater2_Core
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(Updater2)

target_link_libraries(Updater2
    PRIVATE
        project_warnings
        Updater2_Core
        Fileinteractions
        Qt6::Widgets
)

set_target_properties(Updater2 PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)

if (WIN32)
    # Find windeployqt via location of qmake
    block(PROPAGATE WINDEPLOYQT_EXECUTABLE)
        get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
        get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
        find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")
    endblock()

    # Run windeployqt
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET Updater2 POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                    --no-compiler-runtime
                    --verbose 1
                    "$<TARGET_FILE:Updater2>"
            COMMENT "Running windeployqt for Updater2 (Setting up required plugins & DLLs)..."
        )
    else()
        message(WARNING "windeployqt was not found. Will skip deployment of plugins and DLLs")
    endif()
endif()

target_sources(Updater2
    PRIVATE 
        src/main.cpp)

if(WIN32) # TODO: Might no longer be needed
    add_custom_command(TARGET Updater2 POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        $<TARGET_RUNTIME_DLLS:Updater2>
        $<TARGET_FILE_DIR:Updater2>
        COMMAND_EXPAND_LISTS
    )
endif()

#[==[

install(TARGETS Updater2
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Updater2
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)

install(SCRIPT ${deploy_script})

#]==]