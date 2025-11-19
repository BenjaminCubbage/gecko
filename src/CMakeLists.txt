cmake_minimum_required(VERSION 3.16)
project(gecko LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
enable_testing()

if(PRODUCTION)
    if(WIN32)
        string(REPLACE "\\" "/" gecko_user_dir "$ENV{USERPROFILE}/.gecko")
    else()
        set(gecko_user_dir "$ENV{HOME}/.gecko")
    endif()
endif()

option(PRODUCTION "Configure environment paths for production. 
                   This does NOT set the configuration to Release." OFF)

add_subdirectory(shared)
add_subdirectory(api)