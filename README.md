## Building the source code

## Building for Release mode
```bash
conan build . --build=missing
```

## Command to run the HttpServer impl example
```bash
# in release mode
./build/Release/server_impl_bin
```

## Command to run the Tests
```bash
# run tests
./build/Release/server_tests_bin
```

## Generation of coverage reports

```bash
make -C build/Release coverage
```

### Open the coverage report 
```bash
# specific to Linux
xdg-open build/Release/CMakeFiles/server_library.dir/src/out/index.html
```
