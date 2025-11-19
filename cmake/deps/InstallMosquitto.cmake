include(GNUInstallDirs)
include(ExternalProject)

# These are necessary because Mosquitto doesn't link to ws2_32 by default.
if (MSVC)
    set(mosquitto_linker_flags "/DEFAULTLIB:ws2_32.lib /DEFAULTLIB:crypt32.lib")
elseif (MINGW)
    set(mosquitto_linker_flags "-lws2_32 -lcrypt32")
endif()

# Mosquitto version 2.0.19 breaks OPENSSL_INCLUDE_DIR.
# Mosquitto versions 2.0.20+ break WITH_THREADING=OFF.
ExternalProject_Add(mosquitto
    DEPENDS openssl cjson
    GIT_REPOSITORY https://github.com/eclipse-mosquitto/mosquitto.git
    GIT_TAG        v2.0.18
    PREFIX         mosquitto

    CMAKE_ARGS
        -DWITH_WEBSOCKETS=OFF
        -DDOCUMENTATION=OFF
        -DWITH_TLS=ON
        -DWITH_THREADING=OFF
        -DWITH_PLUGINS=ON
        -DWITH_CJSON=ON
        -DCMAKE_EXE_LINKER_FLAGS=${mosquitto_linker_flags}

        -DCJSON_LIBRARY=${CMAKE_INSTALL_FULL_LIBDIR}/cjson${CMAKE_STATIC_LIBRARY_SUFFIX}
        -DCJSON_INCLUDE_DIR=${CMAKE_INSTALL_FULL_INCLUDEDIR}

        -DOPENSSL_ROOT_DIR=${CMAKE_INSTALL_PREFIX}
        -DOPENSSL_INCLUDE_DIR=${CMAKE_INSTALL_FULL_INCLUDEDIR}
        -DOPENSSL_USE_STATIC_LIBS=TRUE

        -DCMAKE_POLICY_VERSION_MINIMUM=3.5

    INSTALL_COMMAND
        ${CMAKE_COMMAND} --install . --config $<CONFIG> --prefix "${CMAKE_INSTALL_PREFIX}"
)