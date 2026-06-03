cmake_minimum_required(VERSION 3.16)
project(pdf_maker VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(PDFMAKER_BUILD_TESTS "Build tests" ON)
option(PDFMAKER_BUILD_EXAMPLES "Build examples" ON)

include(FetchContent)

# zlib (needed by libharu for compression and PNG support)
FetchContent_Declare(
    zlib
    GIT_REPOSITORY https://github.com/madler/zlib.git
    GIT_TAG        v1.3.1
)
set(ZLIB_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(zlib)

# make zlib discoverable by libharu's find_package(ZLIB)
set(ZLIB_FOUND TRUE CACHE BOOL "" FORCE)
set(ZLIB_INCLUDE_DIR "${zlib_SOURCE_DIR};${zlib_BINARY_DIR}" CACHE STRING "" FORCE)
set(ZLIB_LIBRARY zlibstatic CACHE STRING "" FORCE)
set(ZLIB_INCLUDE_DIRS "${zlib_SOURCE_DIR};${zlib_BINARY_DIR}" CACHE STRING "" FORCE)
set(ZLIB_LIBRARIES zlibstatic CACHE STRING "" FORCE)

# create ZLIB::ZLIB imported target alias for libpng compatibility
if(NOT TARGET ZLIB::ZLIB)
    add_library(ZLIB::ZLIB ALIAS zlibstatic)
endif()

# libpng (needed by libharu for PNG image support)
FetchContent_Declare(
    libpng
    GIT_REPOSITORY https://github.com/glennrp/libpng.git
    GIT_TAG        v1.6.43
)
set(PNG_SHARED OFF CACHE BOOL "" FORCE)
set(PNG_STATIC ON CACHE BOOL "" FORCE)
set(PNG_TESTS OFF CACHE BOOL "" FORCE)
set(PNG_BUILD_ZLIB ON CACHE BOOL "" FORCE)
set(SKIP_INSTALL_ALL ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(libpng)
set(SKIP_INSTALL_ALL OFF CACHE BOOL "" FORCE)

# make libpng discoverable by libharu
set(PNG_FOUND TRUE CACHE BOOL "" FORCE)
set(PNG_INCLUDE_DIRS "${libpng_SOURCE_DIR};${libpng_BINARY_DIR}" CACHE STRING "" FORCE)
set(PNG_PNG_INCLUDE_DIR "${libpng_SOURCE_DIR};${libpng_BINARY_DIR}" CACHE STRING "" FORCE)
set(PNG_LIBRARY png_static CACHE STRING "" FORCE)
set(PNG_LIBRARIES png_static CACHE STRING "" FORCE)

FetchContent_Declare(
    libharu
    GIT_REPOSITORY https://github.com/libharu/libharu.git
    GIT_TAG        v2.4.4
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(libharu)

add_subdirectory(src)

if(PDFMAKER_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(PDFMAKER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
