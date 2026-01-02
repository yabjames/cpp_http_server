cmake_minimum_required(VERSION 3.15)
project(server_demo VERSION 1.0 LANGUAGES C CXX)

set(LIBRARY "server_library")
set(TARGET "server_demo_bin")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Override Debug flags
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 --coverage")
set(CMAKE_C_FLAGS_DEBUG "-g -O0 --coverage")
set(CMAKE_EXE_LINKER_FLAGS_DEBUG "--coverage")
set(CMAKE_SHARED_LINKER_FLAGS_DEBUG "--coverage")


# server implementation executable
add_executable(${TARGET}
        main.cpp
)
target_link_libraries(${TARGET} PRIVATE ${LIBRARY})
