include(FetchContent)
include(ExternalProject)
include(GNUInstallDirs)


set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

set(SRC_FILES 
    "${SRC_DIR}/BitOperations.cpp"
    "${SRC_DIR}/BitStream.cpp"
    "${SRC_DIR}/CodeWords.cpp"
    "${SRC_DIR}/CompressedBitonal.cpp"
    "${SRC_DIR}/Decoder.cpp"
    "${SRC_DIR}/Encoder.cpp"
    "${SRC_DIR}/UncompressedBitonal.cpp"
)

set(INC_FILES
    "${INC_DIR}/BitOperations.h"
    "${INC_DIR}/BitStream.h"
    "${INC_DIR}/CodeWords.h"
    "${INC_DIR}/CompressedBitonal.h"
    "${INC_DIR}/Decoder.h"
    "${INC_DIR}/Encoder.h"
    "${INC_DIR}/UncompressedBitonal.h"
)


add_library(compression_lib STATIC)
target_sources(compression_lib PRIVATE ${SRC_FILES} ${INC_FILES})
target_include_directories(compression_lib PUBLIC ${INC_DIR})


set(WASM_SRC_FILES ${SRC_FILES} "${CMAKE_CURRENT_SOURCE_DIR}/bindings/WasmBindings.cpp")


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


add_custom_command(TARGET compression_lib POST_BUILD
    COMMAND echo Building WASM files...

    # `emcc` expects an environment initialized with emsdk activate, but
    # practically speaking the environment variables set by such a script
    # are inevitably ephemeral. Instead, we set %EMSDK_PYTHON% here manually.

    # Technically, there's an %EMSDK_NODE% environment variable that should 
    # be set as well, but as far as I can tell it isn't actually necessary.
    COMMAND ${CMAKE_COMMAND} -E env
        EMSDK_PYTHON=${python_exe_path}
        ${_shell_command} ${emcc_script_path} --std=c++20 -O3 -I "${CMAKE_CURRENT_SOURCE_DIR}/include" --bind ${WASM_SRC_FILES} -o compression.js
    WORKING_DIRECTORY ${GECKO_ROOT}/run/wasm
)


add_subdirectory(tests)