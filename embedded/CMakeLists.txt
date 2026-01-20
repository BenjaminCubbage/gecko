cmake_minimum_required(VERSION 3.14)

set(PICO_PLATFORM rp2350)
set(PICO_BOARD pico2_w)

set(PICO_SDK_FETCH_FROM_GIT ON)
set(PICO_SDK_FETCH_FROM_GIT_TAG "2.2.0")
include(pico_sdk_import.cmake)

project(gecko_embedded LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

pico_sdk_init()
include(FetchContent)

add_subdirectory(../compression compression)

FetchContent_Declare(pimoroni_pico
    GIT_REPOSITORY https://github.com/pimoroni/pimoroni-pico
    GIT_TAG        v1.25.0)

FetchContent_MakeAvailable(pimoroni_pico)

add_executable(embedded
    "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/FS.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/FS.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/FSCert.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/FSCert.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/FSTwoPhase.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/FSTwoPhase.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/Log.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/Log.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/MQTTConn.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/MQTTConn.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/MQTTPub.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/MQTTPub.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/MQTTSub.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/MQTTSub.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/Wifi.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/Wifi.h")

target_link_libraries(embedded PRIVATE
    compression_lib
    fatfs
    hardware_i2c
    inky_frame_7
    pico_cyw43_arch_lwip_poll
    pico_lwip_mbedtls
    pico_lwip_mqtt
    pico_mbedtls
    pico_printf
    pico_stdlib
    sdcard)

target_precompile_headers(embedded PUBLIC
    <string.h>
    <time.h>
    [["hardware/gpio.h"]]
    [["hardware/uart.h"]]
    [["lwip/altcp_tcp.h"]]
    [["lwip/altcp_tls.h"]]
    [["lwip/dns.h"]]
    [["lwip/pbuf.h"]]
    [["lwipopts.h"]]
    [["mbedtls_config.h"]]
    [["pico/cyw43_arch.h"]]
    [["pico/stdio.h"]]
    [["pico/stdlib.h"]])

target_include_directories(embedded PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${pimoroni_pico_SOURCE_DIR})
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