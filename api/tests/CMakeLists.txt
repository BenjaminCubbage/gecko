include(FetchContent)
include(CTest)


set(INSTALL_GTEST OFF CACHE BOOL "")
set(BUILD_GMOCK   OFF CACHE BOOL "")
FetchContent_Declare(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.17.0
)
FetchContent_MakeAvailable(googletest)

set(test_src_dir "${CMAKE_CURRENT_SOURCE_DIR}/src")
set(main_src_dir "${CMAKE_CURRENT_SOURCE_DIR}/../src")

set(test_inc_dir "${CMAKE_CURRENT_SOURCE_DIR}/include")
set(main_inc_dir "${CMAKE_CURRENT_SOURCE_DIR}/../include")

set(src_files 
    "${main_src_dir}/thread/ThreadPool.cpp"
    "${test_src_dir}/ThreadPoolTest.cpp"
)

set(inc_files 
    "${main_inc_dir}/gecko/thread/ThreadPool.h"
)

add_executable(api_tests)
target_sources(api_tests PRIVATE ${src_files} ${inc_files})
target_include_directories(api_tests PUBLIC ${main_inc_dir} ${test_inc_dir})
target_link_libraries(api_tests gtest_main)