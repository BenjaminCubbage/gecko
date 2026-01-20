ExternalProject_Add(emsdk
    PREFIX emsdk
    GIT_REPOSITORY https://github.com/emscripten-core/emsdk.git
    GIT_TAG main
    INSTALL_COMMAND   ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ""
)

ExternalProject_Get_Property(emsdk SOURCE_DIR)

ExternalProject_Add_Step(emsdk emsdk_install
    DEPENDEES download
    COMMAND ./emsdk install 4.0.23
    WORKING_DIRECTORY ${SOURCE_DIR}
)

ExternalProject_Add_Step(emsdk emsdk_active
    DEPENDEES emsdk_install
    COMMAND ./emsdk activate 4.0.23
    WORKING_DIRECTORY ${SOURCE_DIR}
)