## Building the source code

### Building for Release mode
```bash
conan build .
cd build/Release/generators
cmake ../../.. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release

# runs the underlying build system (Makefiles, VStudio solution files)
cmake --build . 
```

### Building for Debug mode
```bash
conan build .
cd build/Debug/generators
cmake ../../.. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug

# runs the underlying build system (Makefiles, VStudio solution files)
cmake --build . 
```

## Command to run the HttpServer impl example
```bash
# in release mode
./build/Release/server_impl_bin

# in debug mode
./build/Debug/server_impl_bin
```

## Command to run the Tests
```bash
# run tests
./build/Debug/server_test_bin
```

## Generation of coverage reports

```bash
cd build/Debug/generators \
&& cmake --build . \
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
