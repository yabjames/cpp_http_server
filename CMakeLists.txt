cmake_minimum_required(VERSION 3.31)
set(TARGET "http_server")
project(${TARGET}
    VERSION 1.0
    LANGUAGES CXX)

find_package(GTest REQUIRED)

add_executable(${TARGET}
    src/main.cpp
    src/HttpServer.cpp
    include/HttpServer.h
    include/AtomicQueue.h
    include/constants.h
)

target_link_libraries(${TARGET}
    gtest::gtest
)

include(GoogleTest)
# gtest_discover_tests(${TARGET})

target_compile_features(${TARGET} PRIVATE cxx_std_17)

if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_compile_options(${TARGET} PRIVATE /W4)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_compile_options(${TARGET} PRIVATE -g -Wall -Wextra -Wpedantic)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # other macOS-specific flags for Clang
endif()
