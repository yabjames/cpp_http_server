## Building the source code

```bash
conan build .

# To build the test executable
conan build . -o test=True
```

## Command to run the Release build

```bash
./build/Release/http_server
```
