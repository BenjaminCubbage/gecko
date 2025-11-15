include(FetchContent)
include(ExternalProject)
include(GNUInstallDirs)


ExternalProject_Add(cjson
    GIT_REPOSITORY https://github.com/DaveGamble/cJSON.git
    GIT_TAG        v1.7.19

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


set(openssl_install_command
    COMMAND ${CMAKE_COMMAND} --install . --config $<CONFIG> --prefix "${CMAKE_INSTALL_PREFIX}"
)

# Don't create a symlink if the installed .lib already has the correct name.
if(NOT openssl_lib_suffix STREQUAL "")
    list(APPEND openssl_install_command
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${CMAKE_INSTALL_FULL_LIBDIR}/libcrypto${openssl_lib_suffix}.${_static_lib_ext}"
            "${CMAKE_INSTALL_FULL_LIBDIR}/libcrypto.${_static_lib_ext}"
        COMMAND ${CMAKE_COMMAND} -E create_symlink
            "${CMAKE_INSTALL_FULL_LIBDIR}/libssl${openssl_lib_suffix}.${_static_lib_ext}"
            "${CMAKE_INSTALL_FULL_LIBDIR}/libssl.${_static_lib_ext}"
    )
endif()


ExternalProject_Add(openssl
    GIT_REPOSITORY https://github.com/janbar/openssl-cmake.git
    GIT_TAG        1.1.1w-20250419

    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DOPENSSL_API_COMPAT=0x10100000L
        -DBUILD_SHARED_LIBS=OFF

    # This is where we actually use that install command from up there.
    INSTALL_COMMAND ${openssl_install_command}
)


# These are necessary because Mosquitto doesn't link to ws2_32 by default.
# Couldn't find another way of forcing mosquitto to link while keeping
# OpenSSL as a static library.
if (MSVC)
    set(mosquitto_linker_flags "/DEFAULTLIB:ws2_32.lib /DEFAULTLIB:crypt32.lib")
elseif (MINGW)
    set(mosquitto_linker_flags "-lws2_32 -lcrypt32")
endif()


# More recent versions of mosquitto break either WITH_THREADING=OFF
# or OPENSSL_INCLUDE_DIR (learned the hard way)
ExternalProject_Add(mosquitto
    DEPENDS cjson openssl

    GIT_REPOSITORY https://github.com/eclipse-mosquitto/mosquitto.git
    GIT_TAG        v2.0.18

    CMAKE_ARGS
        -DWITH_WEBSOCKETS=OFF
        -DDOCUMENTATION=OFF
        -DWITH_TLS=ON
        -DWITH_THREADING=OFF
        -DWITH_PLUGINS=ON
        -DWITH_CJSON=ON
        -DCMAKE_EXE_LINKER_FLAGS=${mosquitto_linker_flags}

        -DCJSON_LIBRARY=${CMAKE_INSTALL_FULL_LIBDIR}/cjson.lib
        -DCJSON_INCLUDE_DIR=${CMAKE_INSTALL_FULL_INCLUDEDIR}

        -DOPENSSL_ROOT_DIR=${CMAKE_INSTALL_PREFIX}
        -DOPENSSL_INCLUDE_DIR=${CMAKE_INSTALL_FULL_INCLUDEDIR}
        -DOPENSSL_USE_STATIC_LIBS=TRUE

        -DCMAKE_POLICY_VERSION_MINIMUM=3.5

    INSTALL_COMMAND
        ${CMAKE_COMMAND} --install . --config $<CONFIG> --prefix "${CMAKE_INSTALL_PREFIX}"
)


FetchContent_Declare(cpphttplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG        v0.27.0
)
FetchContent_MakeAvailable(cpphttplib)


FetchContent_Declare(pahomqtt
    GIT_REPOSITORY https://github.com/eclipse/paho.mqtt.c.git
    GIT_TAG        v1.3.15
)
set(PAHO_BUILD_SHARED   OFF CACHE BOOL "" FORCE)
set(PAHO_BUILD_STATIC   ON  CACHE BOOL "" FORCE)
set(PAHO_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(pahomqtt)


set(mosquitto_config_path_in "${CMAKE_CURRENT_SOURCE_DIR}/mosquitto.conf.in")
set(mosquitto_config_path    "${CMAKE_CURRENT_BINARY_DIR}/mosquitto.conf")
set(mosquitto_pid_path       "${CMAKE_CURRENT_BINARY_DIR}/mosquitto.pid")
set(kill_mosquito_path_in    "${CMAKE_CURRENT_SOURCE_DIR}/kill-mosquitto.${_script_ext}.in")
set(kill_mosquito_path       "${CMAKE_CURRENT_BINARY_DIR}/kill-mosquitto.${_script_ext}")

configure_file("${mosquitto_config_path_in}" "${mosquitto_config_path}")
configure_file("${kill_mosquito_path_in}"    "${kill_mosquito_path}")


set(GECKO_API_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(GECKO_API_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(GECKO_API_SRC_FILES "${GECKO_API_SRC_DIR}/api.cpp")
set(GECKO_API_INC_FILES "${GECKO_API_INC_DIR}/api.h")

add_executable(api)

target_sources(api PRIVATE 
    ${GECKO_API_SRC_FILES} 
    ${GECKO_API_INC_FILES}
)

target_include_directories(api PRIVATE 
    ${cpphttplib_SOURCE_DIR}
    ${pahomqtt_SOURCE_DIR}
    ${GECKO_API_INC_FILES}
)

target_link_libraries(api PRIVATE paho-mqtt3c-static)


add_custom_target(api_kill_mosquitto
    COMMAND ${CMAKE_COMMAND} -E echo Trying to kill old mosquitto.exe instance. This step requires Administrator priviledges.
    COMMAND ${kill_mosquito_path}
)

add_custom_target(api_run_mosquitto
    DEPENDS api_kill_mosquitto mosquitto
    COMMAND ${CMAKE_COMMAND} -E echo Starting mosquitto.exe...
    COMMAND cmd /c "${CMAKE_INSTALL_FULL_SBINDIR}/mosquitto -c ${mosquitto_config_path}"
)

add_custom_target(api_run_main
    DEPENDS api
    COMMAND ${CMAKE_COMMAND} -E echo Starting up HTTP Server...
    COMMAND $<TARGET_FILE_DIR:api>/$<TARGET_FILE_NAME:api>
)