cmake_minimum_required(VERSION 3.16)
project(gecko LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GECKO_EXPECTED_BIN_DIR "${CMAKE_SOURCE_DIR}/out/build")
set(GECKO_ARTIFACTS_DIR     ${CMAKE_SOURCE_DIR}/out/artifacts)

if(NOT CMAKE_BINARY_DIR STREQUAL GECKO_EXPECTED_BIN_DIR)
    message("${BIN_DIR}")
    message("${EXPECTED_BIN_DIR}")
    message(FATAL_ERROR 
        "This project must be built to ${GECKO_EXPECTED_BIN_DIR}.\n"
        "Run from source root: `cmake -B out/build -S .`"
    )
endif()

add_subdirectory(shared)
add_subdirectory(api)