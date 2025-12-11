## Building the source code

### Building for Release mode
```bash
conan build . --build=missing
cmake -S . -B build/Release -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

# runs the underlying build system (Makefiles, VStudio solution files)
cmake --build build/Release
```

## Command to run the HttpServer impl example
```bash
# in release mode
./build/Release/server_impl_bin
```

## Command to run the Tests
```bash
# run tests
./build/Release/server_test_bin
```

## Generation of coverage reports

```bash
cmake --build build/Release \
&& ctest \
&& cd CMakeFiles/server_library.dir/src \
&& gcov HttpServer.cpp.gcno \
&& lcov -c -d . -o coverage.info \
&& genhtml coverage.info -o out
```

### Open the coverage report 
```bash
# specific to Linux
xdg-open build/Debug/generators/CMakeFiles/server_library.dir/src/out/index.html
```
