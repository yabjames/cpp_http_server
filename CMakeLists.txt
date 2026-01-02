cmake_minimum_required(VERSION 3.15)
project(http_server_library VERSION 1.0 LANGUAGES C CXX)

add_subdirectory(demo)

set(LIBRARY "server_library")
set(TEST_TARGET "server_tests_bin")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Override Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 --coverage")
set(CMAKE_C_FLAGS_DEBUG "-g -O0 --coverage")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "--coverage")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "--coverage")

find_package(GTest CONFIG REQUIRED)
find_package(Threads REQUIRED)


# server library
add_library(${LIBRARY} STATIC
        src/HttpServer.cpp
        src/HttpParser.cpp
        src/PathParams.cpp
        include/HttpServer.h
        include/AtomicQueue.h
        include/constants.h
        include/HttpParser.h
        include/PathParams.h
        include/Route.h
)
target_include_directories(${LIBRARY} PUBLIC include)
target_compile_features(${LIBRARY} PRIVATE cxx_std_17)
target_link_libraries(${LIBRARY} PRIVATE Threads::Threads)


# unit tests executable
enable_testing()
add_executable(${TEST_TARGET}
        test/HttpServerTest.cpp
        test/HttpParserTest.cpp
)
target_link_libraries(${TEST_TARGET}
        PRIVATE ${LIBRARY} GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${TEST_TARGET})


# coverage target
# find required tools
find_program(LCOV lcov REQUIRED)
find_program(GENHTML genhtml REQUIRED)

# add coverage target for gcc build
add_custom_target(coverage
        # gather data
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/CMakeFiles/server_library.dir/src
        COMMAND ${LCOV} -c -d . -o coverage.info
        COMMAND ${GENHTML} coverage.info -o out
)
