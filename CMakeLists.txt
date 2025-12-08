cmake_minimum_required(VERSION 3.16)

project(gecko_superbuild LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/external)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
include(GNUInstallDirs)
include(ExternalProject)
set(GECKO_ROOT ${CMAKE_SOURCE_DIR})

include(cmake/install_emsdk.cmake)
option(PRODUCTION "Configure environment paths for production. 
                   This option may cause some files to be written to outside of the repository.
                   This does NOT necessarily set the configuration to Release." OFF)

if(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE MATCHES "")
    set(CMAKE_BUILD_TYPE Debug)
endif()

ExternalProject_Add(gecko_api
    PREFIX gecko_api
    SOURCE_DIR ../api
    INSTALL_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)

# If we use the default configure command CMake doesn't reconfigure when
# the external projects' CMakeLists.txt change.
ExternalProject_Add_Step(gecko_api real_configure
    DEPENDEES configure
    COMMAND ${CMAKE_COMMAND} 
        -B ${CMAKE_BINARY_DIR}/gecko_api/src/gecko_api-build
        -S ${CMAKE_SOURCE_DIR}/api
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DGECKO_ROOT=${GECKO_ROOT}
        -DPRODUCTION=${PRODUCTION}
    DEPENDS 
        ${CMAKE_SOURCE_DIR}/api/CMakeLists.txt
        ${CMAKE_SOURCE_DIR}/api/tests/CMakeLists.txt
)

ExternalProject_Add_Step(gecko_api real_build
    DEPENDEES real_configure
    COMMAND ${CMAKE_COMMAND} 
        --build ${CMAKE_BINARY_DIR}/gecko_api/src/gecko_api-build
        --config $<CONFIG>
    ALWAYS 1
)

ExternalProject_Add(gecko_compression_wasm
    DEPENDS emsdk
    PREFIX gecko_compression_wasm
    SOURCE_DIR ../compression
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
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
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DGECKO_ROOT=${GECKO_ROOT}
        -DPRODUCTION=${PRODUCTION}
        -DWIFI_PASSWORD=${WIFI_PASSWORD}
        -DWIFI_SSID=${WIFI_SSID}
    INSTALL_COMMAND ""
    BUILD_COMMAND cmake --build . --target embedded
    BUILD_ALWAYS 1
)