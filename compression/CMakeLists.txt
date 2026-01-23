cmake_minimum_required(VERSION 3.14)
project(gecko_compression)

include(FetchContent)
include(ExternalProject)
include(GNUInstallDirs)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY         "${GECKO_ARTIFACTS_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   "${GECKO_ARTIFACTS_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${GECKO_ARTIFACTS_DIR}/bin")

set(src_files
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/CodeWords.cpp")

set(inc_files
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/BitOperations.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/BitStream.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/CodeWords.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/Decode.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/gecko/Encode.h")
    
set(compression_wasm_bindings_src 
    "${CMAKE_CURRENT_SOURCE_DIR}/bindings/WasmBindings.cpp")

add_library(compression_lib STATIC)
target_sources(compression_lib PRIVATE ${src_files} ${inc_files})
target_include_directories(compression_lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src")

# EMSDK does not have an installation target, period. Instead, we essentially
# memorize where its executables and scripts are located and point to them here.

# Eventually, this might be broken into its own CMake module, but for now the
# logic is relatively straightfoward so I'm okay with doing it this way. Since
# this project is a superbuild pattern, we can guarantee that the emsdk dependency
# has been installed by the time this project is configured
set(emsdk_root "${GECKO_ROOT_DIR}/build/emsdk/src/emsdk")
set(emcc_script_path "${emsdk_root}/upstream/emscripten/emcc")
set(compression_artifacts_dir "${GECKO_ARTIFACTS_DIR}/wasm")

add_custom_command(
    DEPENDS
        ${src_files}
        ${inc_files}
        ${compression_wasm_bindings_src}

    OUTPUT
        ${compression_artifacts_dir}/compression.js
        ${compression_artifacts_dir}/compression.wasm

    COMMENT "Building WASM files..."
    COMMAND ${CMAKE_COMMAND} -E make_directory ${compression_artifacts_dir}
    COMMAND
        cd ${compression_artifacts_dir} &&
        ${emcc_script_path} --std=c++20 -O3
            -sALLOW_TABLE_GROWTH=1
            -sEXPORTED_RUNTIME_METHODS=addFunction,removeFunction
            -I "${CMAKE_CURRENT_SOURCE_DIR}/src"
            --bind
                ${src_files}
                ${compression_wasm_bindings_src}
            -o compression.js
)

add_custom_target(compression_wasm ALL
    DEPENDS
        ${compression_artifacts_dir}/compression.js
        ${compression_artifacts_dir}/compression.wasm
)

add_subdirectory(tests)