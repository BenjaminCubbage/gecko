cmake_minimum_required(VERSION 3.14)
project(gecko_compression)

include(FetchContent)
include(ExternalProject)
include(GNUInstallDirs)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY         "${GECKO_ROOT}/run/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG   "${GECKO_ROOT}/run/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${GECKO_ROOT}/run/lib")

if(WIN32)
    set(shell_command cmd /c)
else()
    set(shell_command bash -c)
endif()

set(src_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(inc_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(src_files 
    "${src_dir}/BitOperations.cpp"
    "${src_dir}/BitStream.cpp"
    "${src_dir}/CodeWords.cpp"
    "${src_dir}/CompressedBitonal.cpp"
    "${src_dir}/Decoder.cpp"
    "${src_dir}/Encoder.cpp"
    "${src_dir}/UncompressedBitonal.cpp"
)

set(inc_files
    "${inc_dir}/BitOperations.h"
    "${inc_dir}/BitStream.h"
    "${inc_dir}/CodeWords.h"
    "${inc_dir}/CompressedBitonal.h"
    "${inc_dir}/Decoder.h"
    "${inc_dir}/Encoder.h"
    "${inc_dir}/UncompressedBitonal.h"
)

set(wasm_src_files ${src_files} "${CMAKE_CURRENT_SOURCE_DIR}/bindings/WasmBindings.cpp")


add_library(compression_lib STATIC)
target_sources(compression_lib PRIVATE ${src_files} ${inc_files})
target_include_directories(compression_lib PUBLIC ${inc_dir})


add_custom_command(TARGET compression_lib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${GECKO_ROOT}/run/wasm
)


# EMSDK does not have an installation target, period. Instead, we essentially
# memorize where its executables and scripts are located and point to them here.

# Eventually, this might be broken into its own CMake module, but for now the
# logic is relatively straightfoward so I'm okay with doing it this way. Since
# this project is a superbuild pattern, we can guarantee that the emsdk dependency
# has been installed by this project's config time.
set(emsdk_root "${GECKO_ROOT}/build/emsdk/src/emsdk")
if (WIN32)
    set(emcc_script_path ${emsdk_root}/upstream/emscripten/emcc.bat)
    set(python_exe_path  ${emsdk_root}/python/3.13.3_64bit/python.exe)
else()
    set(emcc_script_path ${emsdk_root}/upstream/emscripten/emcc)
    set(python_exe_path  ${emsdk_root}/python/3.13.3_64bit/python3)
endif()


add_custom_command(
    DEPENDS 
        ${src_files}
        ${inc_files}

    OUTPUT 
        ${GECKO_ROOT}/run/wasm/compression.js
        ${GECKO_ROOT}/run/wasm/compression.wasm
    
    # `emcc` expects an environment initialized with emsdk activate, but
    # practically speaking the environment variables set by such a script
    # are inevitably ephemeral. Instead, we set %EMSDK_PYTHON% here manually.

    # Technically, there's an %EMSDK_NODE% environment variable that should
    # be set as well, but as far as I can tell it isn't actually necessary.
    COMMENT "Building WASM files..."
    COMMAND ${CMAKE_COMMAND} -E env
        EMSDK_PYTHON=${python_exe_path}
        ${shell_command} ${emcc_script_path} --std=c++20 -O3 -I "${CMAKE_CURRENT_SOURCE_DIR}/include" --bind ${wasm_src_files} -o compression.js
    
    WORKING_DIRECTORY ${GECKO_ROOT}/run/wasm
)


add_custom_target(compression_wasm ALL
    DEPENDS 
        ${GECKO_ROOT}/run/wasm/compression.js
        ${GECKO_ROOT}/run/wasm/compression.wasm
)


add_subdirectory(tests)