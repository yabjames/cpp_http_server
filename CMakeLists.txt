cmake_minimum_required(VERSION 3.12.0)

set(TARGET "http_server")

project(${TARGET}
    VERSION 1.0
    LANGUAGES CXX)

add_executable(${TARGET}
    src/main.cpp
    src/ThreadPool.cpp
    include/ThreadPool.h
    include/AtomicQueue.h
    include/constants.h)

target_compile_features(${TARGET} PRIVATE cxx_std_20)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_options(${TARGET} PRIVATE /W4)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(${TARGET} PRIVATE -Wall -Wextra -Wpedantic)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # other macOS-specific flags for Clang
endif()
