cmake_minimum_required(VERSION 3.14)

set(PICO_SDK_FETCH_FROM_GIT ON)
set(PICO_SDK_FETCH_FROM_GIT_TAG "2.2.0")
include(pico_sdk_import.cmake)

project(gecko_embedded LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

pico_sdk_init()
add_executable(embedded "src/main.cpp")
target_link_libraries(embedded PRIVATE pico_stdlib pico_printf)
target_precompile_headers(embedded PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/pch.h")
target_include_directories(embedded PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
pico_add_extra_outputs(embedded)

add_custom_command(TARGET embedded POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:embedded>"
        "${GECKO_ROOT}/run/bin/$<TARGET_FILE_NAME:embedded>"
        
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE_DIR:embedded>/embedded.uf2"
        "${GECKO_ROOT}/run/bin/embedded.uf2"
)