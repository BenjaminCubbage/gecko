cmake_minimum_required(VERSION 3.14)
project(gecko_api)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_FIND_LIBRARY_PREFIXES lib "")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY         "${GECKO_ROOT}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${GECKO_ROOT}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${GECKO_ROOT}/bin")

include(FetchContent)
if(WIN32)
    set(gecko_user_dir "$ENV{USERPROFILE}/.gecko")
else()
    set(gecko_user_dir "/var/lib/gecko")
endif()


# Ports
if(PRODUCTION)
    set(api_port        443)
    set(mosquitto_port  1883)
    set(mysql_port      3306)
    set(mysql_xapi_port 33060)
else()
    set(api_port        3001)
    set(mosquitto_port  3002)
    set(mysql_port      3003)
    set(mysql_xapi_port 3004)
endif()


# Mosquitto configuration
set(mosquitto_conf_path        "${GECKO_ROOT}/run/.mosquitto/mosquitto.conf")
set(mosquitto_pid_path         "${GECKO_ROOT}/run/.mosquitto/mosquitto.pid")
set(dynamic_security_conf_path "${gecko_user_dir}/.mosquitto/dynsec.json")

# MySQL
set(mysql_conf_path "${GECKO_ROOT}/run/.mysql/mysqld.cnf")
set(mysql_schema_path "${GECKO_ROOT}/run/.mysql/schema.sql")
set(mysql_socket_path "${gecko_user_dir}/.mysql/mysql.sock")
set(mysql_pid_path "${GECKO_ROOT}/run/.mysql/mysqld.pid")
set(mysql_datadir "${gecko_user_dir}/.mysql/data")

# Backend SSL / Certs
set(pkey_path   "${gecko_user_dir}/.api/certs/pkey.pem")
set(cert_path   "${gecko_user_dir}/.api/certs/cert.pem")
set(pubkey_path "${gecko_user_dir}/.api/certs/pubkey.pem")

# Find programs / shared libs
set(gecko_api_exe_path "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/api${CMAKE_EXECUTABLE_SUFFIX}")
find_program(mosquitto_exe_path        NAMES mosquitto)
find_program(mosquitto_ctrl_exe_path   NAMES mosquitto_ctrl)
find_program(mysqld_exe_path           NAMES mysqld_safe)
find_program(mysql_exe_path            NAMES mysql)
find_program(openssl_exe_path          NAMES openssl)
find_library(dynamic_security_lib_path NAMES mosquitto_dynamic_security)
find_library(libssl_lib_path           NAMES libssl)
find_library(libcrypto_lib_path        NAMES libcrypt)

set(src_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(src_files 
    "${src_dir}/_Main.cpp"
    "${src_dir}/controllers/issuing/Cookies.cpp"
    "${src_dir}/controllers/issuing/Headers.cpp"
    "${src_dir}/controllers/issuing/Issuer.cpp"
    "${src_dir}/controllers/respond/RespondWithError.cpp"
    "${src_dir}/controllers/rules/HasJSONBody.cpp"
    "${src_dir}/controllers/rules/HasJSONValueMember.cpp"
    "${src_dir}/controllers/rules/HasValidOAuthXSRFNonce.cpp"
    "${src_dir}/controllers/rules/HasValidXSRFToken.cpp"
    "${src_dir}/controllers/rules/IsSuccessfulOAuthCallback.cpp"
    "${src_dir}/controllers/rules/PathParamEquals.cpp"
    "${src_dir}/controllers/rules/UserIsLoggedIn.cpp"
    "${src_dir}/controllers/AuthController.cpp"
    "${src_dir}/controllers/UsersController.cpp"
    "${src_dir}/db/ConnectionPool.cpp"
    "${src_dir}/db/Users.cpp"
    "${src_dir}/json/MergePatch.cpp"
    "${src_dir}/server/Server.cpp"
    "${src_dir}/env/Env.cpp"
    "${src_dir}/env/EnvParse.cpp"
    "${src_dir}/env/EnvPopulate.cpp"
    "${src_dir}/fs/ReadWholeFile.cpp"
    "${src_dir}/services/UsersService.cpp"
    "${src_dir}/thread/ThreadPool.cpp"
    "${src_dir}/util/ParseHeader.cpp"
    "${src_dir}/util/UUID.cpp"
)

set(inc_files 
    "${inc_dir}/gecko/_Main.h"
    "${inc_dir}/gecko/controllers/issuing/Cookies.h"
    "${inc_dir}/gecko/controllers/issuing/Headers.h"
    "${inc_dir}/gecko/controllers/issuing/Issuer.h"
    "${inc_dir}/gecko/controllers/respond/RespondWithError.h"
    "${inc_dir}/gecko/controllers/rules/HasJSONBody.h"
    "${inc_dir}/gecko/controllers/rules/HasJSONValueMember.h"
    "${inc_dir}/gecko/controllers/rules/HasValidOAuthXSRFNonce.h"
    "${inc_dir}/gecko/controllers/rules/HasValidXSRFToken.h"
    "${inc_dir}/gecko/controllers/rules/IsSuccessfulOAuthCallback.h"
    "${inc_dir}/gecko/controllers/rules/PathParamEquals.h"
    "${inc_dir}/gecko/controllers/rules/UserIsLoggedIn.h"
    "${inc_dir}/gecko/controllers/AuthController.h"
    "${inc_dir}/gecko/controllers/UsersController.h"
    "${inc_dir}/gecko/db/ConnectionPool.h"
    "${inc_dir}/gecko/db/Users.h"
    "${inc_dir}/gecko/env/Env.h"
    "${inc_dir}/gecko/env/EnvParse.h"
    "${inc_dir}/gecko/env/EnvPopulate.h"
    "${inc_dir}/gecko/fs/ReadWholeFile.h"
    "${inc_dir}/gecko/json/MergePatch.h"
    "${inc_dir}/gecko/server/Server.h"
    "${inc_dir}/gecko/models/User.h"
    "${inc_dir}/gecko/models/UserPatch.h"
    "${inc_dir}/gecko/services/UsersService.h"
    "${inc_dir}/gecko/thread/ThreadPool.h"
    "${inc_dir}/gecko/util/ParseHeader.h"
    "${inc_dir}/gecko/util/UUID.h"
)

set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE} CACHE STRING "" FORCE)
set(PAHO_BUILD_SHARED   OFF CACHE BOOL "" FORCE)
set(PAHO_BUILD_STATIC   ON  CACHE BOOL "" FORCE)
set(PAHO_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC        ON  CACHE BOOL "" FORCE)
set(JSONCPP_WITH_TESTS  OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS   OFF CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS   ON  CACHE BOOL "" FORCE)
set(JWT_SSL_LIBRARY "OpenSSL" CACHE STRING "" FORCE)
set(JWT_DISABLE_PICOJSON TRUE CACHE  BOOL "" FORCE)
set(JWT_BUILD_EXAMPLES   OFF  CACHE  BOOL "" FORCE)

FetchContent_Declare(pahomqtt
    GIT_REPOSITORY https://github.com/eclipse/paho.mqtt.c.git
    GIT_TAG        v1.3.15
)

FetchContent_Declare(cpphttplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG        v0.27.0
)

FetchContent_Declare(mysqlconnectorcpp
    GIT_REPOSITORY https://github.com/mysql/mysql-connector-cpp.git
    GIT_TAG        9.5.0
)

FetchContent_Declare(jsoncpp
    GIT_REPOSITORY https://github.com/open-source-parsers/jsoncpp.git
    GIT_TAG        1.9.6
)

FetchContent_Declare(jwtcpp
    GIT_REPOSITORY https://github.com/Thalhammer/jwt-cpp.git
    GIT_TAG        v0.7.1
)

FetchContent_MakeAvailable(cpphttplib)
FetchContent_MakeAvailable(pahomqtt)
FetchContent_MakeAvailable(mysqlconnectorcpp)
FetchContent_MakeAvailable(jsoncpp)
set(jsoncpp_DIR "${jsoncpp_SOURCE_DIR}" CACHE STRING "" FORCE)
FetchContent_MakeAvailable(jwtcpp)

add_executable(api)
target_sources(api PRIVATE 
    ${src_files} 
    ${inc_files}
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(api PRIVATE -ggdb -fno-inline-small-functions -fno-inline)
    target_link_options(api PRIVATE -ggdb)
endif()

target_compile_definitions(api PRIVATE 
    -DCPPHTTPLIB_OPENSSL_SUPPORT
)

target_include_directories(api PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include/api
    ${CMAKE_BINARY_DIR}/include/mysqlx
    ${cpphttplib_SOURCE_DIR}
    ${pahomqtt_SOURCE_DIR}
)

target_link_libraries(api PRIVATE 
    paho-mqtt3c-static 
    connector
    jsoncpp_static
    jwt-cpp

    ${libssl_lib_path}
    ${libcrypto_lib_path}
)

add_subdirectory(tests)