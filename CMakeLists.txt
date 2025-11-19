cmake_minimum_required(VERSION 3.16)
project(gecko_superbuild LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/external)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
include(GNUInstallDirs)
include(ExternalProject)

include(cmake/deps/InstallCJSON.cmake)
include(cmake/deps/InstallOpenSSL.cmake)
include(cmake/deps/InstallMosquitto.cmake)
include(cmake/deps/InstallEMSDK.cmake)

add_custom_target(all_external_dependencies DEPENDS mosquitto emsdk)

# We assume external dependencies have been installed by the time we target gecko.
# That way we skip checking for updated files that will never exist.
ExternalProject_Add(gecko
    PREFIX gecko
    SOURCE_DIR ../src
    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DGECKO_ROOT=${CMAKE_SOURCE_DIR}
    INSTALL_COMMAND ""
    BUILD_ALWAYS 1
)