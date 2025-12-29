cmake_minimum_required(VERSION 3.15)
project(http_server VERSION 1.0 LANGUAGES C CXX)

set(LIBRARY "server_library")
set(TEST_TARGET "server_tests_bin")
set(TARGET "server_impl_bin")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(GTest REQUIRED)
find_package(Threads REQUIRED)


# server library
add_library(${LIBRARY}
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
target_compile_options(${LIBRARY} PRIVATE -O0 -g --coverage)
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
target_compile_options(${TEST_TARGET} PRIVATE -g -O0 -g --coverage)
target_link_options(${TEST_TARGET} PRIVATE -O0 -g --coverage)


# server implementation executable
add_executable(${TARGET}
        src/main.cpp
)
target_compile_options(${TARGET} PRIVATE -g -O0 -g --coverage)
target_link_options(${TARGET} PRIVATE -O0 -g --coverage)
target_link_libraries(${TARGET} PRIVATE ${LIBRARY})

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
