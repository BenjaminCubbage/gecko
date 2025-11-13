include(FetchContent)

FetchContent_Declare(emsdk
    GIT_REPOSITORY https://github.com/emscripten-core/emsdk
    GIT_TAG main
)
FetchContent_MakeAvailable(emsdk)

if(WIN32)
    set(SHELL_COMMAND cmd /c)
    set(EMSDK_PYTHON_PATH ${emsdk_SOURCE_DIR}/python/3.13.3_64bit/python.exe)
else()
    set(SHELL_COMMAND bash -c)
    set(EMSDK_PYTHON_PATH ${emsdk_SOURCE_DIR}/python/3.13.3_64bit/python3)
endif()

if(NOT EXISTS "${emsdk_SOURCE_DIR}/upstream")
    message("Installing EMSDK...")

    execute_process(
        COMMAND ${SHELL_COMMAND} emsdk install latest
        WORKING_DIRECTORY "${emsdk_SOURCE_DIR}"
    )
endif()

if(NOT EXISTS "${emsdk_SOURCE_DIR}/.emscripten")
    message("${emsdk_SOURCE_DIR}/.emscripten")

    execute_process(
        COMMAND ${SHELL_COMMAND} emsdk activate latest
        WORKING_DIRECTORY "${emsdk_SOURCE_DIR}"
    )
endif()

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

add_custom_command(TARGET compression_lib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${GECKO_ARTIFACTS_DIR}/wasm
)

find_program(GECKO_EMCC_COMMAND emcc HINTS "${emsdk_SOURCE_DIR}/upstream/emscripten")

add_custom_command(TARGET compression_lib POST_BUILD
    COMMAND echo Building WASM files...
    COMMAND ${CMAKE_COMMAND} -E env
        EMSDK_PYTHON=${EMSDK_PYTHON_PATH}
        ${SHELL_COMMAND} ${GECKO_EMCC_COMMAND} --std=c++20 -O3 -I "${CMAKE_CURRENT_SOURCE_DIR}/include" --bind ${WASM_SRC_FILES} -o compression.js
    WORKING_DIRECTORY ${GECKO_ARTIFACTS_DIR}/wasm
)

add_subdirectory(tests)