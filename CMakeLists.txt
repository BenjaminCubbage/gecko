cmake_minimum_required(VERSION 3.16)
project(gecko_superbuild LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INSTALL_PREFIX ${CMAKE_SOURCE_DIR}/external)
set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
cmake_policy(SET CMP0114 NEW)

include(GNUInstallDirs)
include(ExternalProject)
set(GECKO_ROOT_DIR ${CMAKE_SOURCE_DIR})
set(GECKO_ARTIFACTS_DIR ${GECKO_ROOT_DIR}/build/artifacts)

include(cmake/install_emsdk.cmake)

if(NOT DEFINED CMAKE_BUILD_TYPE OR CMAKE_BUILD_TYPE MATCHES "")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# note(ben): configure_project / build_project external project steps are
# used instead of the default configure / build targets due to the fact that
# CMake does not provide an easy way to specify the file dependencies of
# the step that runs CONFIGURE_COMMAND. Because of this, and for reasons
# not entirely known to me, CMake was reconfiguring the whole project each
# and every time the project was targetted, even if no files had changed.

#
# API
#

ExternalProject_Add(api
    PREFIX api
    SOURCE_DIR ../api
    INSTALL_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)

ExternalProject_Get_Property(api BINARY_DIR)
ExternalProject_Get_Property(api SOURCE_DIR)

ExternalProject_Add_Step(api configure_project
    DEPENDEES configure
    COMMAND ${CMAKE_COMMAND}
        -G ${CMAKE_GENERATOR}
        -B ${BINARY_DIR}
        -S ${SOURCE_DIR}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DGECKO_ARTIFACTS_DIR=${GECKO_ROOT_DIR}/build/artifacts
    DEPENDS
        ${CMAKE_SOURCE_DIR}/api/CMakeLists.txt
        ${CMAKE_SOURCE_DIR}/api/tests/CMakeLists.txt
    # ALWAYS 1
)

ExternalProject_Add_Step(api build_project
    DEPENDEES configure_project
    COMMAND ${CMAKE_COMMAND} 
        --build ${BINARY_DIR}
        --config $<CONFIG>
        --target api
    ALWAYS 1
)

ExternalProject_Add_Step(api build_tests
    DEPENDEES configure_project
    COMMAND ${CMAKE_COMMAND} 
        --build ${BINARY_DIR}
        --config $<CONFIG>
        --target api_tests
    ALWAYS 1
)

ExternalProject_Add_StepTargets(api build_project build_tests)

#
# Compression WASM
#

ExternalProject_Add(compression_wasm
    DEPENDS emsdk
    PREFIX compression_wasm
    SOURCE_DIR ../compression
    CMAKE_ARGS
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DGECKO_ROOT_DIR=${GECKO_ROOT_DIR}
        -DGECKO_ARTIFACTS_DIR=${GECKO_ROOT_DIR}/build/artifacts
    BUILD_COMMAND cmake --build . --target compression_wasm
    INSTALL_COMMAND ""
    BUILD_ALWAYS 1
)

ExternalProject_Add(embedded
    PREFIX embedded
    SOURCE_DIR ../embedded
    INSTALL_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
)

ExternalProject_Get_Property(embedded BINARY_DIR)
ExternalProject_Get_Property(embedded SOURCE_DIR)

ExternalProject_Add_Step(embedded configure_project
    DEPENDEES configure
    COMMAND ${CMAKE_COMMAND}
        -G ${CMAKE_GENERATOR}
        -B ${BINARY_DIR}
        -S ${SOURCE_DIR}
        -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
        -DGECKO_ARTIFACTS_DIR=${GECKO_ROOT_DIR}/build/artifacts
        -DWIFI_SSID=${WIFI_SSID}
        -DWIFI_PASSWORD=${WIFI_PASSWORD}
    DEPENDS
        ${CMAKE_SOURCE_DIR}/embedded/CMakeLists.txt
    # ALWAYS 1
)

ExternalProject_Add_Step(embedded build_project
    DEPENDEES configure_project
    COMMAND ${CMAKE_COMMAND} 
        --build ${BINARY_DIR}
        --config $<CONFIG>
        --target embedded
    ALWAYS 1
)