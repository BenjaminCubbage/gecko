if(WIN32)
    set(shell_command cmd /c)
else()
    set(shell_command bash -c)
endif()

ExternalProject_Add(emsdk
    PREFIX emsdk
    GIT_REPOSITORY https://github.com/emscripten-core/emsdk.git
    GIT_TAG main
    INSTALL_COMMAND   ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ""
)

ExternalProject_Get_Property(emsdk SOURCE_DIR)

if (WIN32)
    ExternalProject_Add_Step(emsdk install_latest
        DEPENDEES download
        COMMAND ${shell_command} emsdk install latest
        WORKING_DIRECTORY ${SOURCE_DIR}
    )

    ExternalProject_Add_Step(emsdk activate_latest
        DEPENDEES install_latest
        COMMAND ${shell_command} emsdk activate latest
        WORKING_DIRECTORY ${SOURCE_DIR}
    )
else()
    ExternalProject_Add_Step(emsdk install_latest
        DEPENDEES download
        COMMAND ./emsdk install latest
        WORKING_DIRECTORY ${SOURCE_DIR}
    )

    ExternalProject_Add_Step(emsdk activate_latest
        DEPENDEES install_latest
        COMMAND ./emsdk activate latest
        WORKING_DIRECTORY ${SOURCE_DIR}
    )
endif()