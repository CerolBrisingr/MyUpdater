
add_library(Updater2_Core)
target_sources(Updater2_Core
    PUBLIC
        hello.cpp)

target_include_directories(Updater2_Core
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include)

qt_standard_project_setup()

qt_add_executable(Updater2)

target_link_libraries(Updater2
    PRIVATE
        Updater2_Core
        Fileinteractions)

target_sources(Updater2
    PRIVATE 
        main.cpp)

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