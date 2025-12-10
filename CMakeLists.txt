cmake_minimum_required(VERSION 3.31)
project(http_server VERSION 1.0 LANGUAGES CXX)

set(LIBRARY "server_library")
set(TEST_TARGET "server_tests_bin")
set(TARGET "server_impl_bin")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# server library
add_library(${LIBRARY}
    src/HttpServer.cpp
    include/HttpServer.h
    include/AtomicQueue.h
    include/constants.h
)
target_include_directories(${LIBRARY} PUBLIC include)
target_compile_features(${LIBRARY} PRIVATE cxx_std_17)
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_options(${LIBRARY} PRIVATE -O0 -g --coverage)
endif()

# unit tests executable
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    enable_testing()
    find_package(GTest REQUIRED)
    add_executable(${TEST_TARGET} test/HttpServerTest.cpp)
    target_link_libraries(${TEST_TARGET}
        PRIVATE ${LIBRARY} GTest::gtest_main
    )

    include(GoogleTest)
    gtest_discover_tests(${TEST_TARGET})
    target_compile_options(${TEST_TARGET} PRIVATE -g -O0 -g --coverage)
    target_link_options(${TEST_TARGET} PRIVATE -O0 -g --coverage)
endif()

# server implementation executable
add_executable(${TARGET}
    src/main.cpp
)
target_link_libraries(${TARGET} PRIVATE ${LIBRARY})
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    target_compile_options(${TARGET} PRIVATE -g -O0 -g --coverage)
    target_link_options(${TARGET} PRIVATE -O0 -g --coverage)
endif()
