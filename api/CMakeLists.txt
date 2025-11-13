include(FetchContent)
include(ExternalProject)

# More recent versions of mosquitto break both WITH_TLS=OFF and WITH_THREADING=OFF.
# CMake won't build older versions.
# v2.0.19 is the only version that will build.
ExternalProject_Add(mosquitto
    GIT_REPOSITORY https://github.com/eclipse-mosquitto/mosquitto.git
    GIT_TAG v2.0.19
    CMAKE_ARGS -DWITH_WEBSOCKETS=OFF
               -DDOCUMENTATION=OFF
               -DWITH_TLS=OFF
               -DWITH_THREADING=FALSE
    EXCLUDE_FROM_ALL
)
ExternalProject_Get_Property(mosquitto BINARY_DIR)

FetchContent_Declare(cpphttplib
    GIT_REPOSITORY https://github.com/yhirose/cpp-httplib.git
    GIT_TAG v0.27.0
)
FetchContent_MakeAvailable(cpphttplib)

FetchContent_Declare(pahomqtt
    GIT_REPOSITORY https://github.com/eclipse/paho.mqtt.c.git
    GIT_TAG        v1.3.15
)
# There is probably a better way to set these
set(PAHO_BUILD_SHARED   OFF CACHE BOOL "" FORCE)
set(PAHO_BUILD_STATIC   ON  CACHE BOOL "" FORCE)
set(PAHO_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(pahomqtt)


set(GECKO_API_SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(GECKO_API_INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(GECKO_API_SRC_FILES "${GECKO_API_SRC_DIR}/api.cpp")
set(GECKO_API_INC_FILES "${GECKO_API_INC_DIR}/api.h")

add_executable(api)

target_sources(api PRIVATE 
    ${GECKO_API_SRC_FILES} 
    ${GECKO_API_INC_FILES}
)

target_include_directories(api PRIVATE 
    ${cpphttplib_SOURCE_DIR}
    ${pahomqtt_SOURCE_DIR}
    ${GECKO_API_INC_FILES}
)

target_link_libraries(api PRIVATE paho-mqtt3c-static)

set(GECKO_MOSQUITTO_CONFIG_PATH_IN "${CMAKE_CURRENT_SOURCE_DIR}/mosquitto.conf.in")
set(GECKO_MOSQUITTO_CONFIG_PATH    "${CMAKE_CURRENT_BINARY_DIR}/mosquitto.conf")
set(GECKO_MOSQUITTO_PID_PATH       "${CMAKE_CURRENT_BINARY_DIR}/mosquitto-pid.temp")
if(WIN32)
set(GECKO_KILL_MOSQUITTO_PATH_IN "${CMAKE_CURRENT_SOURCE_DIR}/kill-mosquitto.bat.in")
set(GECKO_KILL_MOSQUITTO_PATH    "${CMAKE_CURRENT_BINARY_DIR}/kill-mosquitto.bat")
else()
set(GECKO_KILL_MOSQUITTO_PATH_IN "${CMAKE_CURRENT_SOURCE_DIR}/kill-mosquitto.sh.in")
set(GECKO_KILL_MOSQUITTO_PATH    "${CMAKE_CURRENT_BINARY_DIR}/kill-mosquitto.sh")
endif()

configure_file("${GECKO_MOSQUITTO_CONFIG_PATH_IN}" "${GECKO_MOSQUITTO_CONFIG_PATH}")
configure_file("${GECKO_KILL_MOSQUITTO_PATH_IN}"   "${GECKO_KILL_MOSQUITTO_PATH}")

add_custom_target(api_kill_mosquitto
    COMMAND ${CMAKE_COMMAND} -E echo Trying to kill old mosquitto.exe instance. This step requires Administrator priviledges.
    COMMAND ${GECKO_KILL_MOSQUITTO_PATH}
)

add_custom_target(api_run_mosquitto
    DEPENDS api_kill_mosquitto mosquitto
    COMMAND ${CMAKE_COMMAND} -E echo Starting mosquitto.exe...
    COMMAND cmd /c "${BINARY_DIR}/src/$<CONFIG>/mosquitto -c ${GECKO_MOSQUITTO_CONFIG_PATH}"
)

add_custom_target(api_run_main
    DEPENDS api
    COMMAND ${CMAKE_COMMAND} -E echo Starting up HTTP Server...
    COMMAND $<TARGET_FILE_DIR:api>/$<TARGET_FILE_NAME:api>
)