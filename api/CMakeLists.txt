cmake_minimum_required(VERSION 3.14)
project(gecko_api)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_FIND_LIBRARY_PREFIXES lib "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY         "${GECKO_ROOT}/run/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${GECKO_ROOT}/run/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${GECKO_ROOT}/run/bin")

include(FetchContent)
if(WIN32)
    set(gecko_user_dir "$ENV{USERPROFILE}/.gecko")
else()
    set(gecko_user_dir "/var/lib/gecko")
endif()


# Mosquitto configuration
set(mosquitto_conf_path        "${GECKO_ROOT}/run/.mosquitto/mosquitto.conf")
set(mosquitto_pid_path         "${GECKO_ROOT}/run/.mosquitto/mosquitto.pid")
set(dynamic_security_conf_path "${gecko_user_dir}/.mosquitto/dynsec.json")
if(PRODUCTION)
    set(mosquitto_port 1883)
else()
    set(mosquitto_port 3002)
endif()

# MySQL
set(mysql_conf_path "${GECKO_ROOT}/run/.mysql/mysqld.cnf")
set(mysql_pid_path "${GECKO_ROOT}/run/.mysql/mysqld.pid")
set(mysql_datadir "${gecko_user_dir}/.mysql/data")
if(PRODUCTION)
    set(mysql_port 3306)
else()
    set(mysql_port 3003)
endif()

# Find programs / shared libs
set(gecko_api_exe_path "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/api${CMAKE_EXECUTABLE_SUFFIX}")
find_program(mosquitto_exe_path        NAMES mosquitto)
find_program(mosquitto_ctrl_exe_path   NAMES mosquitto_ctrl)
find_program(mysqld_exe_path           NAMES mysqld_safe)
find_program(mysql_exe_path            NAMES mysql)
find_library(dynamic_security_lib_path NAMES mosquitto_dynamic_security)

set(src_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(src_files "${src_dir}/api.cpp")
set(inc_files "${inc_dir}/api.h")

configure_file("${GECKO_ROOT}/cmake/local.env.in" "${GECKO_ROOT}/run/local.env")
configure_file("${GECKO_ROOT}/cmake/mosquitto.conf.in" "${mosquitto_conf_path}")
configure_file("${GECKO_ROOT}/cmake/mysqld.cnf.in" "${mysql_conf_path}" FILE_PERMISSIONS OWNER_READ GROUP_READ WORLD_READ)


set(PAHO_BUILD_SHARED   OFF CACHE BOOL "" FORCE)
set(PAHO_BUILD_STATIC   ON  CACHE BOOL "" FORCE)
set(PAHO_ENABLE_TESTING OFF CACHE BOOL "" FORCE)

FetchContent_Declare(pahomqtt
    GIT_REPOSITORY https://github.com/eclipse/paho.mqtt.c.git
    GIT_TAG        v1.3.15
)

FetchContent_Declare(cpphttplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG        v0.27.0
)

FetchContent_MakeAvailable(cpphttplib)
FetchContent_MakeAvailable(pahomqtt)

add_executable(api)
target_sources(api PRIVATE 
    ${src_files} 
    ${inc_files}
)

target_compile_definitions(api PRIVATE -DMOSQUITTO_PORT=${mosquitto_port})

target_include_directories(api PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${cpphttplib_SOURCE_DIR}
    ${pahomqtt_SOURCE_DIR}
)

target_link_libraries(api PRIVATE paho-mqtt3c-static)