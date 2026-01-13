include(FetchContent)
include(CTest)

set(INSTALL_GTEST OFF CACHE BOOL "")
set(BUILD_GMOCK   OFF CACHE BOOL "")
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

set(test_src_dir "${CMAKE_CURRENT_SOURCE_DIR}")
set(main_src_dir "${CMAKE_CURRENT_SOURCE_DIR}/../src")

set(src_files
    "${test_src_dir}/http/MultipartFormDataTest.cpp"
    "${test_src_dir}/http/ParseHeaderTest.cpp"
    "${test_src_dir}/http/ParseValueTest.cpp"
    "${test_src_dir}/http/RespondWithErrorTest.cpp"
    "${test_src_dir}/middleware/HasContentLengthLTETest.cpp"
    "${test_src_dir}/middleware/HasHeaderTest.cpp"
    "${test_src_dir}/middleware/HasJSONBodyTest.cpp"
    "${test_src_dir}/middleware/HasMultipartFormDataFieldTest.cpp"
    "${test_src_dir}/middleware/HasPathParamTest.cpp"
    "${test_src_dir}/middleware/HasValidOAuthXSRFNonceTest.cpp"
    "${test_src_dir}/middleware/HasValidXSRFTokenTest.cpp"
    "${test_src_dir}/middleware/IsSuccessfulOAuthCallbackTest.cpp"
    "${test_src_dir}/middleware/PathParamEqualsTest.cpp"
    "${test_src_dir}/middleware/UserIsLoggedInTest.cpp"
    "${test_src_dir}/rand/UUIDTest.cpp"
    "${test_src_dir}/thread/ThreadPoolTest.cpp"
)

add_executable(api_tests ${src_files})
target_link_libraries(api_tests PRIVATE gtest_main api_core)