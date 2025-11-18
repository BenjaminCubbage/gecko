cmake_minimum_required(VERSION 3.16)
project(gecko_superbuild LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/external)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
include(GNUInstallDirs)
include(ExternalProject)

if(WIN32)
    set(_shell_command cmd /c)
    set(_script_ext bat)
    set(_static_lib_ext lib)
    set(_shared_lib_ext dll)
    set(_executable_dot_ext .exe)
else()
    set(_shell_command bash -c)
    set(_script_ext sh)
    set(_static_lib_ext a)
    set(_shared_lib_ext so)
    set(_executable_dot_ext "")
endif()

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


# OpenSSL-CMake gets fancy with how it names its exported lib file.
# Sometimes it exports as libcrypto-1_1-x64.lib, sometimes not.
#
# The issue is that FindOpenSSL() always looks for libssl.lib and
# libcrypto.lib.
#
# This logic is duplicated from the repo because there's no simple
# way to programmatically extract the target.
if(ANDROID)
    set(openssl_lib_suffix "_1_1")
elseif(MSVC)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(openssl_lib_suffix "-1_1-x64")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(openssl_lib_suffix "-1_1")
    endif()
else()
    set(openssl_lib_suffix "")
endif()

ExternalProject_Add(openssl
    GIT_REPOSITORY https://github.com/janbar/openssl-cmake.git
    GIT_TAG        1.1.1w-20250419
    PREFIX         openssl

    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DBUILD_SHARED_LIBS=OFF
        -DWITH_APPS=OFF

    INSTALL_COMMAND 
        COMMAND ${CMAKE_COMMAND} --install . --config $<CONFIG> --prefix "${CMAKE_INSTALL_PREFIX}"
)

if(NOT openssl_lib_suffix STREQUAL "")
    ExternalProject_Add_Step(openssl create_symlinks
        COMMAND "${CMAKE_COMMAND}" -E create_symlink
            "${CMAKE_INSTALL_FULL_LIBDIR}/libcrypto${openssl_lib_suffix}.${_static_lib_ext}"
            "${CMAKE_INSTALL_FULL_LIBDIR}/libcrypto.${_static_lib_ext}"
        COMMAND "${CMAKE_COMMAND}" -E create_symlink
            "${CMAKE_INSTALL_FULL_LIBDIR}/libssl${openssl_lib_suffix}.${_static_lib_ext}"
            "${CMAKE_INSTALL_FULL_LIBDIR}/libssl.${_static_lib_ext}"
        DEPENDEES install
    )
endif()


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

        -DCJSON_LIBRARY=${CMAKE_INSTALL_FULL_LIBDIR}/cjson.${_static_lib_ext}
        -DCJSON_INCLUDE_DIR=${CMAKE_INSTALL_FULL_INCLUDEDIR}

        -DOPENSSL_ROOT_DIR=${CMAKE_INSTALL_PREFIX}
        -DOPENSSL_INCLUDE_DIR=${CMAKE_INSTALL_FULL_INCLUDEDIR}
        -DOPENSSL_USE_STATIC_LIBS=TRUE

        -DCMAKE_POLICY_VERSION_MINIMUM=3.5

    INSTALL_COMMAND
        ${CMAKE_COMMAND} --install . --config $<CONFIG> --prefix "${CMAKE_INSTALL_PREFIX}"
)


ExternalProject_Add(emsdk
    PREFIX emsdk
    GIT_REPOSITORY https://github.com/emscripten-core/emsdk.git
    GIT_TAG main
    INSTALL_COMMAND   ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ""
)

ExternalProject_Get_Property(emsdk SOURCE_DIR)
set(emsdk_source_dir ${SOURCE_DIR})

ExternalProject_Add_Step(emsdk install_latest
    DEPENDEES download
    COMMAND ${_shell_command} emsdk install latest
    WORKING_DIRECTORY ${SOURCE_DIR}
)

ExternalProject_Add_Step(emsdk activate_latest
    DEPENDEES install_latest
    COMMAND ${_shell_command} emsdk activate latest
    WORKING_DIRECTORY ${SOURCE_DIR}
)

add_custom_target(all_external_dependencies DEPENDS mosquitto emsdk)


# We assume external dependencies have been installed by the time we target gecko.
# This allows us to skip checking for 
ExternalProject_Add(gecko
    PREFIX gecko
    SOURCE_DIR ../src
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX:string=${CMAKE_INSTALL_PREFIX}
        -DGECKO_ROOT:string=${CMAKE_SOURCE_DIR}
    INSTALL_COMMAND ""
    BUILD_ALWAYS 1
)