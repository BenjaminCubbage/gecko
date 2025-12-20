cmake_minimum_required(VERSION 3.14)
project(gecko_api)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_FIND_LIBRARY_PREFIXES lib "")
include(FetchContent)

set(gecko_user_dir "/var/lib/gecko")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY         "${GECKO_ARTIFACTS_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${GECKO_ARTIFACTS_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${GECKO_ARTIFACTS_DIR}/bin")

find_library(libssl_lib_path    NAMES libssl)
find_library(libcrypto_lib_path NAMES libcrypt)

set(src_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")

set(src_files
    "${src_dir}/gecko/controllers/AuthController.cpp"
    "${src_dir}/gecko/controllers/AuthController.h"
    "${src_dir}/gecko/controllers/DevicesController.cpp"
    "${src_dir}/gecko/controllers/DevicesController.h"
    "${src_dir}/gecko/controllers/SharedImagesController.cpp"
    "${src_dir}/gecko/controllers/SharedImagesController.h"
    "${src_dir}/gecko/controllers/UsersController.cpp"
    "${src_dir}/gecko/controllers/UsersController.h"
    "${src_dir}/gecko/db/ConnectionPool.cpp"
    "${src_dir}/gecko/db/ConnectionPool.h"
    "${src_dir}/gecko/db/DevicesTable.cpp"
    "${src_dir}/gecko/db/DevicesTable.h"
    "${src_dir}/gecko/db/SharedImagesTable.cpp"
    "${src_dir}/gecko/db/SharedImagesTable.h"
    "${src_dir}/gecko/db/UsersTable.cpp"
    "${src_dir}/gecko/db/UsersTable.h"
    "${src_dir}/gecko/env/Env.cpp"
    "${src_dir}/gecko/env/Env.h"
    "${src_dir}/gecko/env/EnvParse.cpp"
    "${src_dir}/gecko/env/EnvParse.h"
    "${src_dir}/gecko/env/EnvPopulate.cpp"
    "${src_dir}/gecko/env/EnvPopulate.h"
    "${src_dir}/gecko/fs/ReadWholeFile.cpp"
    "${src_dir}/gecko/fs/ReadWholeFile.h"
    "${src_dir}/gecko/http/Constants.cpp"
    "${src_dir}/gecko/http/Constants.h"
    "${src_dir}/gecko/http/MultipartFormData.cpp"
    "${src_dir}/gecko/http/MultipartFormData.h"
    "${src_dir}/gecko/http/ParseHeader.cpp"
    "${src_dir}/gecko/http/ParseHeader.h"
    "${src_dir}/gecko/http/RespondWithError.cpp"
    "${src_dir}/gecko/http/RespondWithError.h"
    "${src_dir}/gecko/json/MergePatch.cpp"
    "${src_dir}/gecko/json/MergePatch.h"
    "${src_dir}/gecko/middleware/HasAnyMultipartFormData.cpp"
    "${src_dir}/gecko/middleware/HasAnyMultipartFormData.h"
    "${src_dir}/gecko/middleware/HasContentLength.cpp"
    "${src_dir}/gecko/middleware/HasContentLength.h"
    "${src_dir}/gecko/middleware/HasHeader.cpp"
    "${src_dir}/gecko/middleware/HasHeader.h"
    "${src_dir}/gecko/middleware/HasJSONBody.cpp"
    "${src_dir}/gecko/middleware/HasJSONBody.h"
    "${src_dir}/gecko/middleware/HasJSONValueMember.cpp"
    "${src_dir}/gecko/middleware/HasJSONValueMember.h"
    "${src_dir}/gecko/middleware/HasMultipartFormDataField.cpp"
    "${src_dir}/gecko/middleware/HasMultipartFormDataField.h"
    "${src_dir}/gecko/middleware/HasPathParam.cpp"
    "${src_dir}/gecko/middleware/HasPathParam.h"
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
    "${src_dir}/gecko/mqtt/MQTTClient.cpp"
    "${src_dir}/gecko/mqtt/MQTTClient.h"
    "${src_dir}/gecko/rand/UUID.cpp"
    "${src_dir}/gecko/rand/UUID.h"
    "${src_dir}/gecko/server/Server.cpp"
    "${src_dir}/gecko/server/Server.h"
    "${src_dir}/gecko/services/DevicesService.cpp"
    "${src_dir}/gecko/services/DevicesService.h"
    "${src_dir}/gecko/services/SharedImagesService.cpp"
    "${src_dir}/gecko/services/SharedImagesService.h"
    "${src_dir}/gecko/services/UsersService.cpp"
    "${src_dir}/gecko/services/UsersService.h"
    "${src_dir}/gecko/thread/ThreadPool.cpp"
    "${src_dir}/gecko/thread/ThreadPool.h"
    "${src_dir}/gecko/topics/DevicesStatusTopic.cpp"
    "${src_dir}/gecko/topics/DevicesStatusTopic.h"
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

# mysql doesn't like it if the CMAKE_RUNTIME_OUTPUT_DIRECTORY is set before
# it's been included
set(saved_output_dir ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})
unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG)
unset(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE)

FetchContent_MakeAvailable(mysqlconnectorcpp)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY         "${saved_output_dir}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   "${saved_output_dir}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${saved_output_dir}")

FetchContent_MakeAvailable(jsoncpp)
set(jsoncpp_DIR "${jsoncpp_SOURCE_DIR}" CACHE STRING "" FORCE)
FetchContent_MakeAvailable(jwtcpp)

# Core

add_library(api_core STATIC)
target_sources(api_core PUBLIC ${src_files})

target_include_directories(api_core PUBLIC
    ${src_dir}
    ${CMAKE_BINARY_DIR}/include/mysqlx
    ${cpphttplib_SOURCE_DIR}
    ${pahomqtt_SOURCE_DIR}
)

target_compile_definitions(api_core PUBLIC
    -DCPPHTTPLIB_OPENSSL_SUPPORT
)

target_precompile_headers(api_core PUBLIC
    "<atomic>"
    "<fstream>"
    "<memory>"
    "<optional>"
    "<string>"
    "<thread>"
    "<unordered_map>"
    "<vector>"
    "<array>"
    [["httplib.h"]]
    [["jwt-cpp/jwt.h"]]
    [["jwt-cpp/traits/open-source-parsers-jsoncpp/traits.h"]]
    [["json/json.h"]]
    [["mysqlx/xdevapi.h"]]
)

target_link_libraries(api_core PUBLIC
    paho-mqtt3as-static
    connector
    jsoncpp_static
    jwt-cpp
    ${libssl_lib_path}
    ${libcrypto_lib_path}
)

# Main

add_executable(api "${src_dir}/main.cpp")
target_link_libraries(api PRIVATE api_core)

if("Debug" STREQUAL "Debug")
    target_compile_options(api PRIVATE -ggdb -fno-inline-small-functions -fno-inline -O0)
    target_link_options(api PRIVATE -ggdb)
else()
    target_compile_options(api PRIVATE -O3)
endif()

# Tests

add_subdirectory(tests)