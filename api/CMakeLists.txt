cmake_minimum_required(VERSION 3.14)
project(gecko_api)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_FIND_LIBRARY_PREFIXES lib "")
include(FetchContent)
if(WIN32)
    set(gecko_user_dir "$ENV{USERPROFILE}/.gecko")
else()
    set(gecko_user_dir "/var/lib/gecko")
endif()

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY         "${GECKO_ARTIFACTS_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${GECKO_ARTIFACTS_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${GECKO_ARTIFACTS_DIR}/bin")

# Mosquitto configuration
set(mosquitto_conf_path        "${GECKO_ROOT_DIR}/run/.mosquitto/mosquitto.conf")
set(mosquitto_pid_path         "${GECKO_ROOT_DIR}/run/.mosquitto/mosquitto.pid")
set(dynamic_security_conf_path "${gecko_user_dir}/.mosquitto/dynsec.json")

# MySQL
set(mysql_conf_path "${GECKO_ROOT_DIR}/run/.mysql/mysqld.cnf")
set(mysql_schema_path "${GECKO_ROOT_DIR}/run/.mysql/schema.sql")
set(mysql_socket_path "${gecko_user_dir}/.mysql/mysql.sock")
set(mysql_pid_path "${GECKO_ROOT_DIR}/run/.mysql/mysqld.pid")
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

set(src_files
    "${src_dir}/main.cpp"
    "${src_dir}/gecko/http/RespondWithError.cpp"
    "${src_dir}/gecko/http/RespondWithError.h"
    "${src_dir}/gecko/http/Constants.cpp"
    "${src_dir}/gecko/http/Constants.h"
    "${src_dir}/gecko/middleware/HasJSONBody.cpp"
    "${src_dir}/gecko/middleware/HasJSONBody.h"
    "${src_dir}/gecko/middleware/HasJSONValueMember.cpp"
    "${src_dir}/gecko/middleware/HasJSONValueMember.h"
    "${src_dir}/gecko/middleware/HasValidOAuthXSRFNonce.cpp"
    "${src_dir}/gecko/middleware/HasValidOAuthXSRFNonce.h"
    "${src_dir}/gecko/middleware/HasValidXSRFToken.cpp"
    "${src_dir}/gecko/middleware/HasValidXSRFToken.h"
    "${src_dir}/gecko/middleware/IsSuccessfulOAuthCallback.cpp"
    "${src_dir}/gecko/middleware/IsSuccessfulOAuthCallback.h"
    "${src_dir}/gecko/middleware/PathParamEquals.cpp"
    "${src_dir}/gecko/middleware/PathParamEquals.h"
    "${src_dir}/gecko/middleware/UserIsLoggedIn.cpp"
    "${src_dir}/gecko/middleware/UserIsLoggedIn.h"
    "${src_dir}/gecko/controllers/AuthController.cpp"
    "${src_dir}/gecko/controllers/AuthController.h"
    "${src_dir}/gecko/controllers/UsersController.cpp"
    "${src_dir}/gecko/controllers/UsersController.h"
    "${src_dir}/gecko/db/ConnectionPool.cpp"
    "${src_dir}/gecko/db/ConnectionPool.h"
    "${src_dir}/gecko/db/UsersTable.cpp"
    "${src_dir}/gecko/db/UsersTable.h"
    "${src_dir}/gecko/json/MergePatch.cpp"
    "${src_dir}/gecko/json/MergePatch.h"
    "${src_dir}/gecko/server/Server.cpp"
    "${src_dir}/gecko/server/Server.h"
    "${src_dir}/gecko/env/Env.cpp"
    "${src_dir}/gecko/env/Env.h"
    "${src_dir}/gecko/env/EnvParse.cpp"
    "${src_dir}/gecko/env/EnvParse.h"
    "${src_dir}/gecko/env/EnvPopulate.cpp"
    "${src_dir}/gecko/env/EnvPopulate.h"
    "${src_dir}/gecko/fs/ReadWholeFile.cpp"
    "${src_dir}/gecko/fs/ReadWholeFile.h"
    "${src_dir}/gecko/services/UsersService.cpp"
    "${src_dir}/gecko/services/UsersService.h"
    "${src_dir}/gecko/thread/ThreadPool.cpp"
    "${src_dir}/gecko/thread/ThreadPool.h"
    "${src_dir}/gecko/util/ParseHeader.cpp"
    "${src_dir}/gecko/util/ParseHeader.h"
    "${src_dir}/gecko/util/UUID.cpp"
    "${src_dir}/gecko/util/UUID.h"
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
    ${src_dir}
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