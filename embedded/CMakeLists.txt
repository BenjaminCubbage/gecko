cmake_minimum_required(VERSION 3.14)

set(PICO_PLATFORM rp2350)
set(PICO_BOARD pico2_w)

set(PICO_SDK_FETCH_FROM_GIT ON)
set(PICO_SDK_FETCH_FROM_GIT_TAG "2.2.0")
include(pico_sdk_import.cmake)

project(gecko_embedded LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(src_dir "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko")
pico_sdk_init()
include(FetchContent)

FetchContent_Declare(pimoroni_pico
    GIT_REPOSITORY https://github.com/pimoroni/pimoroni-pico
    GIT_TAG        v1.25.0)
    
FetchContent_MakeAvailable(pimoroni_pico)

add_executable(embedded
    "${src_dir}/FS.cpp"
    "${src_dir}/FS.h"
    "${src_dir}/FSCert.cpp"
    "${src_dir}/FSCert.h"
    "${src_dir}/Log.cpp"
    "${src_dir}/Log.h"
    "${src_dir}/main.cpp"
    "${src_dir}/MQTTConn.cpp"
    "${src_dir}/MQTTConn.h"
    "${src_dir}/MQTTPub.cpp"
    "${src_dir}/MQTTPub.h"
    "${src_dir}/Wifi.cpp"
    "${src_dir}/Wifi.h")

target_link_libraries(embedded PRIVATE
    fatfs
    sdcard
    pico_mbedtls
    pico_stdlib
    pico_printf
    pico_lwip_mqtt
    pico_lwip_mbedtls
    pico_cyw43_arch_lwip_poll)

target_precompile_headers(embedded PUBLIC
    <string.h>
    <time.h>
    [["mbedtls_config.h"]]
    [["lwipopts.h"]]
    [["pico/stdlib.h"]]
    [["pico/stdio.h"]]
    [["pico/cyw43_arch.h"]]
    [["hardware/uart.h"]]
    [["hardware/gpio.h"]]
    [["lwip/pbuf.h"]]
    [["lwip/altcp_tcp.h"]]
    [["lwip/altcp_tls.h"]]
    [["lwip/dns.h"]])

target_include_directories(embedded PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${pimoroni_pico_SOURCE_DIR}/drivers)
pico_add_extra_outputs(embedded)

target_compile_definitions(embedded PRIVATE
    PICO_DEFAULT_UART=0
    PICO_DEFAULT_UART_TX_PIN=0
    PICO_DEFAULT_UART_RX_PIN=1
    PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS=100
    WIFI_PASSWORD="${WIFI_PASSWORD}"
    WIFI_SSID="${WIFI_SSID}"
    MQTT_IP="${MQTT_IP}"
    MQTT_PORT=${MQTT_PORT})

pico_enable_stdio_usb(embedded 1)
pico_enable_stdio_uart(embedded 0)

add_custom_command(TARGET embedded POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:embedded>"
        "${GECKO_ARTIFACTS_DIR}/bin/$<TARGET_FILE_NAME:embedded>"

    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE_DIR:embedded>/embedded.uf2"
        "${GECKO_ARTIFACTS_DIR}/bin/embedded.uf2")