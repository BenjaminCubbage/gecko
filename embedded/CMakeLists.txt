cmake_minimum_required(VERSION 3.14)

set(PICO_BOARD pico_w)
set(PICO_SDK_FETCH_FROM_GIT ON)
set(PICO_SDK_FETCH_FROM_GIT_TAG "2.2.0")
include(pico_sdk_import.cmake)

project(gecko_embedded LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions("-DWIFI_PASSWORD=\"${WIFI_PASSWORD}\"" "-DWIFI_SSID=\"${WIFI_SSID}\"")

pico_sdk_init()
add_executable(embedded "src/main.cpp" "src/tls.cpp")
target_link_libraries(embedded PRIVATE 
    pico_mbedtls 
    pico_stdlib
    pico_printf
    pico_lwip_mbedtls
    pico_cyw43_arch_lwip_poll
)
target_precompile_headers(embedded PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include/pch.h")
target_include_directories(embedded PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
pico_add_extra_outputs(embedded)

target_compile_definitions(embedded PRIVATE
    PICO_DEFAULT_UART=0
    PICO_DEFAULT_UART_TX_PIN=0
    PICO_DEFAULT_UART_RX_PIN=1
)

add_custom_command(TARGET embedded POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:embedded>"
        "${GECKO_ROOT_DIR}/run/bin/$<TARGET_FILE_NAME:embedded>"
        
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE_DIR:embedded>/embedded.uf2"
        "${GECKO_ROOT_DIR}/run/bin/embedded.uf2"
)