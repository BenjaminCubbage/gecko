add_library(compression_lib STATIC)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")
target_sources(compression_lib PRIVATE
    "${SRC_DIR}/BitOperations.cpp"
    "${INC_DIR}/BitOperations.h"
    "${SRC_DIR}/BitStream.cpp"
    "${INC_DIR}/BitStream.h"
    "${SRC_DIR}/CodeWords.cpp"
    "${INC_DIR}/CodeWords.h"
    "${SRC_DIR}/CompressedBitonal.cpp"
    "${INC_DIR}/CompressedBitonal.h"
    "${SRC_DIR}/Decoder.cpp"
    "${INC_DIR}/Decoder.h"
    "${SRC_DIR}/Encoder.cpp"
    "${INC_DIR}/Encoder.h"
    "${SRC_DIR}/UncompressedBitonal.cpp"
    "${INC_DIR}/UncompressedBitonal.h"
)

target_include_directories(compression_lib PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/"
)

add_subdirectory(tests)