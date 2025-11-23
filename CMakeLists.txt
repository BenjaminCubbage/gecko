cmake_minimum_required(VERSION 3.16)

project(gecko_superbuild LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/external)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
include(GNUInstallDirs)
include(ExternalProject)
set(GECKO_ROOT ${CMAKE_SOURCE_DIR})

include(cmake/deps/install_emsdk.cmake)
option(PRODUCTION "Configure environment paths for production. 
                   This option may cause some files to be written to outside of the repository.
                   This does NOT necessarily set the configuration to Release." OFF)

ExternalProject_Add(gecko_api
    PREFIX gecko_api
    SOURCE_DIR ../api
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DGECKO_ROOT=${GECKO_ROOT}
        -DPRODUCTION=${PRODUCTION}
    INSTALL_COMMAND ""
    BUILD_ALWAYS 1
)

ExternalProject_Add(gecko_compression_wasm
    PREFIX gecko_compression_wasm
    SOURCE_DIR ../compression
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DGECKO_ROOT=${GECKO_ROOT}
        -DPRODUCTION=${PRODUCTION}
    BUILD_COMMAND cmake --build . --target compression_wasm
    INSTALL_COMMAND ""
    BUILD_ALWAYS 1
)

ExternalProject_Add(gecko_embedded
    PREFIX gecko_embedded
    SOURCE_DIR ../embedded
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DGECKO_ROOT=${GECKO_ROOT}
        -DPRODUCTION=${PRODUCTION}
        -DWIFI_PASSWORD=${WIFI_PASSWORD}
        -DWIFI_SSID=${WIFI_SSID}
    INSTALL_COMMAND ""
    BUILD_COMMAND cmake --build . --target embedded
    BUILD_ALWAYS 1
)