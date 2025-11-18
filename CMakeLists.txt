cmake_minimum_required(VERSION 3.16)
project(gecko LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()

if(WIN32)
    set(_shell_command cmd /c)
    set(_script_ext bat)
    set(_static_lib_ext lib)
    set(_shared_lib_ext dll)
    set(_executable_dot_ext .exe)
else()
    set(_shell_command bash -c)
    set(_script_ext sh)
    set(_static_lib_ext a)
    set(_shared_lib_ext so)
    set(_executable_dot_ext "")
endif()

set(gecko_expected_build_dir "${CMAKE_SOURCE_DIR}/out/build")
get_filename_component(CMAKE_INSTALL_PREFIX "${CMAKE_BINARY_DIR}/../install" REALPATH)

if(NOT CMAKE_BINARY_DIR STREQUAL gecko_expected_build_dir)
    message(FATAL_ERROR 
        "This project must be built to ${gecko_expected_build_dir}.\n"
        "Run from source root: `cmake -B out/build -S .`"
    )
endif()

add_subdirectory(api)
add_subdirectory(shared)