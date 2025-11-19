include(ExternalProject)

ExternalProject_Add(cjson
    GIT_REPOSITORY https://github.com/DaveGamble/cJSON.git
    GIT_TAG        v1.7.19
    PREFIX         cjson

    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DBUILD_SHARED_LIBS=OFF
        -DENABLE_CJSON_TEST=OFF
        -DENABLE_CJSON_UTILS=OFF
        -DCMAKE_POLICY_VERSION_MINIMUM=3.5

    INSTALL_COMMAND
        ${CMAKE_COMMAND} --install . --config $<CONFIG> --prefix "${CMAKE_INSTALL_PREFIX}"
)