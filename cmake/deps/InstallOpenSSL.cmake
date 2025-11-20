include(GNUInstallDirs)
include(ExternalProject)

# OpenSSL-CMake gets fancy with how it names its exported lib file.
# Sometimes it exports as libcrypto-1_1-x64.lib, sometimes not.

# The issue is that FindOpenSSL() always looks for libssl.lib and
# libcrypto.lib.

# This logic is duplicated from the repo because there's no simple
# way to programmatically extract the target, especially not at
# config time.
if(ANDROID)
    set(openssl_lib_suffix "_1_1")
elseif(MSVC)
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(openssl_lib_suffix "-1_1-x64")
    elseif(CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(openssl_lib_suffix "-1_1")
    endif()
else()
    set(openssl_lib_suffix "")
endif()

# This would be under prod circumstances a bad repo to clone from, because
# it's unofficial and unmaintained. However, for this small project it's
# convenient to have a cmake wrapper for OpenSSL out of the box.
ExternalProject_Add(openssl
    GIT_REPOSITORY https://github.com/janbar/openssl-cmake.git
    GIT_TAG        1.1.1w-20250419
    PREFIX         openssl

    CMAKE_ARGS
        -DCMAKE_INSTALL_PREFIX=${CMAKE_INSTALL_PREFIX}
        -DBUILD_SHARED_LIBS=OFF
        -DWITH_APPS=OFF
        -DCMAKE_POSITION_INDEPENDENT_CODE=ON

    INSTALL_COMMAND 
        COMMAND ${CMAKE_COMMAND} --install . --config $<CONFIG> --prefix "${CMAKE_INSTALL_PREFIX}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_INSTALL_FULL_LIBDIR}/libcrypto${openssl_lib_suffix}${CMAKE_STATIC_LIBRARY_SUFFIX}"
            "${CMAKE_INSTALL_FULL_LIBDIR}/libcrypto${CMAKE_STATIC_LIBRARY_SUFFIX}"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CMAKE_INSTALL_FULL_LIBDIR}/libssl${openssl_lib_suffix}${CMAKE_STATIC_LIBRARY_SUFFIX}"
            "${CMAKE_INSTALL_FULL_LIBDIR}/libssl${CMAKE_STATIC_LIBRARY_SUFFIX}"
)