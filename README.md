## Building the source code

```bash
conan build .

# To build the test executable
conan build . -o test=True
```

## Command to run the Release build

```bash
./build/Release/server_impl_bin

# run tests
./build/Release/server_test_bin
```

## Generation of coverage reports

```bash
cd build/Release \
&& ctest \
&& cd CMakeFiles/server_library.dir/src \
&& gcov HttpServer.cpp.gcno \
&& lcov -c -d . -o coverage.info \
&& genhtml coverage.info -o out
```
