set(CMAKE_RUNTIME_OUTPUT_DIRECTORY         "${GECKO_ROOT}/run/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${GECKO_ROOT}/run/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${GECKO_ROOT}/run/bin")
set(CMAKE_INSTALL_PREFIX                   "${GECKO_ROOT}/external")

include(FetchContent)
include(ExternalProject)
include(GNUInstallDirs)

set(PAHO_BUILD_SHARED   OFF CACHE BOOL "" FORCE)
set(PAHO_BUILD_STATIC   ON  CACHE BOOL "" FORCE)
set(PAHO_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

# Mosquitto configuration
set(mosquitto_conf_path        "${GECKO_ROOT}/run/.mosquitto/mosquitto.conf")
set(mosquitto_pid_path         "${GECKO_ROOT}/run/.mosquitto/mosquitto.pid")
if(PRODUCTION)
set(dynamic_security_conf_path "${gecko_user_dir}/.mosquitto/dynsec.json")
else()
set(dynamic_security_conf_path "${GECKO_ROOT}/run/.mosquitto/dynsec.json")
endif()

# Programs / shared libs
set(gecko_api_exe_path        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/api${CMAKE_EXECUTABLE_SUFFIX}")
set(mosquitto_exe_path        "${CMAKE_INSTALL_FULL_SBINDIR}/mosquitto${CMAKE_EXECUTABLE_SUFFIX}")
set(mosquitto_ctrl_exe_path   "${CMAKE_INSTALL_FULL_BINDIR}/mosquitto_ctrl${CMAKE_EXECUTABLE_SUFFIX}")
set(dynamic_security_lib_path "${CMAKE_INSTALL_FULL_BINDIR}/mosquitto_dynamic_security${CMAKE_SHARED_LIBRARY_SUFFIX}")

set(src_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(src_files "${src_dir}/api.cpp")
set(inc_files "${inc_dir}/api.h")

configure_file("${GECKO_ROOT}/cmake/local.env.in" "${GECKO_ROOT}/run/local.env")
configure_file("${GECKO_ROOT}/cmake/mosquitto.conf.in" "${mosquitto_conf_path}")


FetchContent_Declare(cpphttplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG        v0.27.0
)

FetchContent_Declare(pahomqtt
    GIT_REPOSITORY https://github.com/eclipse/paho.mqtt.c.git
    GIT_TAG        v1.3.15
)

FetchContent_MakeAvailable(cpphttplib)
FetchContent_MakeAvailable(pahomqtt)


add_executable(api)
target_sources(api PRIVATE 
    ${src_files} 
    ${inc_files}
)

target_include_directories(api PRIVATE
    ${cpphttplib_SOURCE_DIR}
    ${pahomqtt_SOURCE_DIR}
    ${inc_files}
)

target_link_libraries(api PRIVATE paho-mqtt3c-static)