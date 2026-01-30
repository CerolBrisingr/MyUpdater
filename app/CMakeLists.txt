
add_library(Updater2_Core)
target_sources(Updater2_Core
    PUBLIC
        src/hello.cpp)

target_include_directories(Updater2_Core
    PUBLIC
        ${CMAKE_CURRENT_LIST_DIR}/include)

qt_standard_project_setup()

qt_add_executable(Updater2)

target_link_libraries(Updater2
    PRIVATE
        Updater2_Core
        Fileinteractions)

target_sources(Updater2
    PRIVATE 
        src/main.cpp)

if(WIN32)
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