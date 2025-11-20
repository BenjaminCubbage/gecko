cmake_minimum_required(VERSION 3.14)

set(PICO_SDK_FETCH_FROM_GIT ON)
set(PICO_SDK_FETCH_FROM_GIT_TAG "2.2.0")

include(pico_sdk_import.cmake)
project(gecko_embedded LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
set(FETCHCONTENT_QUIET FALSE)

pico_sdk_init()
add_executable(embedded "src/main.cpp")
target_link_libraries(embedded PRIVATE pico_stdlib pico_printf)
target_precompile_headers(embedded PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/pch.h")
target_include_directories(embedded PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
pico_add_extra_outputs(embedded)