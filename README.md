## Building the source code

```bash
conan install . --output-folder=build --build=missing
cd build

# CMAKE_TOOLCHAIN_FILE generator translates the current package configuration, settings, and options, into CMake toolchain syntax
# CMAKE_BUILD_TYPE is the Release mode
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

# runs the underlying build system (Makefiles, VStudio solution files)
cmake --build .

```

## Command to run the executable

```bash
./build/http_server
```
