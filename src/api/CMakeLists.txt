set(CMAKE_RUNTIME_OUTPUT_DIRECTORY         "${GECKO_ROOT}/run/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${GECKO_ROOT}/run/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${GECKO_ROOT}/run/bin")
set(CMAKE_INSTALL_PREFIX                   "${GECKO_ROOT}/external")

include(FetchContent)
include(ExternalProject)
include(GNUInstallDirs)


FetchContent_Declare(cpphttplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG        v0.27.0
)
FetchContent_MakeAvailable(cpphttplib)


set(PAHO_BUILD_SHARED   OFF CACHE BOOL "" FORCE)
set(PAHO_BUILD_STATIC   ON  CACHE BOOL "" FORCE)
set(PAHO_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_Declare(pahomqtt
    GIT_REPOSITORY https://github.com/eclipse/paho.mqtt.c.git
    GIT_TAG        v1.3.15
)
FetchContent_MakeAvailable(pahomqtt)


# Mosquitto configuration
set(mosquitto_conf_path        "${GECKO_ROOT}/run/.mosquitto/mosquitto.conf")
set(mosquitto_pid_path         "${GECKO_ROOT}/run/.mosquitto/mosquitto.pid")
set(dynamic_security_conf_path "${GECKO_ROOT}/run/.mosquitto/dynsec.json")

# Programs / shared libs
set(mosquitto_exe_path        "${CMAKE_INSTALL_FULL_SBINDIR}/mosquitto${_executable_dot_ext}")
set(dynamic_security_lib_path "${CMAKE_INSTALL_FULL_BINDIR}/mosquitto_dynamic_security.${_shared_lib_ext}")
set(mosquitto_ctrl_exe_path   "${CMAKE_INSTALL_FULL_BINDIR}/mosquitto_ctrl${_executable_dot_ext}")

configure_file(
    "${GECKO_ROOT}/cmake/mosquitto.conf.in" 
    "${mosquitto_conf_path}"
    @ONLY
)

configure_file(
    "${GECKO_ROOT}/cmake/scripts.env.in" 
    "${GECKO_ROOT}/scripts/scripts.env"
    @ONLY
)


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