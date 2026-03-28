cmake_minimum_required(VERSION 3.16)
project(pdfmaker VERSION 0.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(BUILD_TESTS "Build tests" ON)
option(BUILD_EXAMPLES "Build examples" ON)

include(FetchContent)

FetchContent_Declare(
    libharu
    GIT_REPOSITORY https://github.com/libharu/libharu.git
    GIT_TAG v2.4.4
)

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(libharu)

add_subdirectory(src)

if (BUILD_TESTS)
    add_subdirectory(tests)
endif()

if (BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
