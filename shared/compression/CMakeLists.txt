include(FetchContent)
include(ExternalProject)
include(GNUInstallDirs)


ExternalProject_Add(emsdk
    GIT_REPOSITORY https://github.com/emscripten-core/emsdk
    GIT_TAG main
    INSTALL_COMMAND   ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND     ""
)

ExternalProject_Get_Property(emsdk SOURCE_DIR)
set(emsdk_source_dir ${SOURCE_DIR})

ExternalProject_Add_Step(emsdk install_latest
    COMMAND ${_shell_command} emsdk install latest
    WORKING_DIRECTORY ${emsdk_source_dir}
    DEPENDEES download
)

ExternalProject_Add_Step(emsdk activate_latest
    COMMAND ${_shell_command} emsdk activate latest
    WORKING_DIRECTORY ${emsdk_source_dir}
    DEPENDEES install_latest
)


# find_program() doesn't work here because emcc and python only exist
# at build time.
if (WIN32)
    set(emcc_command ${emsdk_source_dir}/upstream/emscripten/emcc.bat)
    set(emsdk_python_path ${emsdk_source_dir}/python/3.13.3_64bit/python.exe)
else()
    set(emcc_command ${emsdk_source_dir}/upstream/emscripten/emcc)
    set(emsdk_python_path ${emsdk_source_dir}/python/3.13.3_64bit/python3)
endif()


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
add_dependencies(compression_lib emsdk)


set(WASM_SRC_FILES ${SRC_FILES} "${CMAKE_CURRENT_SOURCE_DIR}/bindings/WasmBindings.cpp")


add_custom_command(TARGET compression_lib POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_INSTALL_FULL_DATADIR}
)


# We're already installing ExternalProject dependencies at build time
# so we may as well install the wasm shim as build time, too.
add_custom_command(TARGET compression_lib POST_BUILD
    COMMAND echo Building WASM files...
    COMMAND ${CMAKE_COMMAND} -E env
        EMSDK_PYTHON=${emsdk_python_path}
        ${_shell_command} ${emcc_command} --std=c++20 -O3 -I "${CMAKE_CURRENT_SOURCE_DIR}/include" --bind ${WASM_SRC_FILES} -o compression.js
    WORKING_DIRECTORY ${CMAKE_INSTALL_FULL_DATADIR}
)


add_subdirectory(tests)