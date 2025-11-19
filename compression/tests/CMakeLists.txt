include(FetchContent)
include(CTest)

FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
set(INSTALL_GTEST OFF CACHE BOOL "")
set(BUILD_GMOCK   OFF CACHE BOOL "")
FetchContent_MakeAvailable(googletest)

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(INC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(compression_tests
    ${SRC_DIR}/BitOperationsTest.cpp
    ${SRC_DIR}/BitStreamTest.cpp
    ${SRC_DIR}/CompressedBitonalTest.cpp
    ${SRC_DIR}/RoundTripBitonalCompressionTest.cpp
    ${SRC_DIR}/UncompressedBitonalTest.cpp
)

target_link_libraries(compression_tests PRIVATE 
    compression_lib
    gtest_main
)

target_include_directories(compression_tests PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)

set_target_properties(compression_tests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/tests/$<CONFIG>"
)

add_custom_command(
    TARGET compression_tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Copying test assets to $<TARGET_FILE_DIR:compression_tests>/assets"
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_CURRENT_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:compression_tests>/assets"
)

add_test(NAME compression_tests 
    COMMAND "$<TARGET_FILE:compression_tests>"
    WORKING_DIRECTORY "$<TARGET_FILE_DIR:compression_tests>"
)