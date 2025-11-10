add_library(compression_lib STATIC)

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

set(WASM_SRC_FILES ${SRC_FILES} "${CMAKE_CURRENT_SOURCE_DIR}/bindings/WasmBindings.cpp")

target_sources(compression_lib PRIVATE ${SRC_FILES} ${INC_FILES})

target_include_directories(compression_lib PUBLIC ${INC_DIR})

find_program(EMCC_EXECUTABLE emcc
    PATHS ENV PATH
)

if (!EMCC_EXECUTABLE)
    message(WARNING "Could not find emcc at config time, please make sure Emscripten is installed and activated on your machine.")
endif()

if(!ENV{EMSDK_PYTHON})
    message(WARNING "EMSDK_PYTHON was not defined at config time.")
    message(WARNING "Run emsdk_env.bat before configuring, or set EMSDK_PYTHON manually.")
else()
    add_custom_command(TARGET compression_lib POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/wasm
        COMMAND
            if not defined EMSDK_PYTHON (
                echo Environment variable EMSDK_PYTHON was not defined at build time. Skipping Emscripten WASM build.
            )
            else (
                echo Running emcc in build directory... &&
                emcc --std=c++20 -O3 -I "${CMAKE_CURRENT_SOURCE_DIR}/include" --bind ${WASM_SRC_FILES} -o compression.js
            )
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/wasm
    )
endif()

add_subdirectory(tests)