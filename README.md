# C++ HTTP Server

A modern C++ HTTP server implementation with routing, testing, and CI-driven quality checks.

---

## Table of Contents

* [Features](#features)
* [Usage](#usage)
* [Project Structure](#project-structure)
* [Building the Source Code](#building-the-source-code)

    * [Building for Release Mode](#building-for-release-mode)
* [Running the Server](#running-the-server)
* [Running the Tests](#running-the-tests)
* [Code Coverage](#code-coverage)
* [CI / GitHub Actions](#ci--github-actions)
* [Project Management](#project-management)
* [Key Skills/Lessons Learned](#key-skillslessons-learned)
* [Challenges](#challenges)

---

## Features

* **Route compilation and matching** with support for path parameters
  * Easily map functions to a new route
* **Automated test suite** using GoogleTest (gtest) with CI/CD pipeline
* **Code coverage reporting** using `lcov`
    * Automated code coverage with CI/CD pipeline
* **GitHub Actions CI pipeline**
    * Build verification
    * Automated test execution
    * Coverage report generation
* **Project tracking** using GitHub Projects (Kanban board)

---

## Usage

```cpp
#include "HttpServer.h"

HttpServer server{};

server.get_mapping(
    "/test", [](const HttpServer::Request &, HttpServer::Response &res) {
        res.body = "testing new api route";
    });

server.post_mapping(
    "/test2/{id}", [](const HttpServer::Request &, HttpServer::Response &res) {
        std::stringstream ss;
        try {
            ss << req.path_params.get_path_param("id").value() << "\n";
            res.body = ss.str();
        } catch (const std::bad_optional_access &e) {
            res.body = "could not get path parameter foo";
        }
    });

try {
    server.listen(3490); // use any port you like
} catch (const std::exception &err) {
    std::cerr << err.what() << '\n';
    return EXIT_FAILURE;
}
```

## Project Structure

```
.
├── src/                # Server implementation
├── include/            # Public headers
├── tests/              # gtest-based test suite
├── cmake/              # CMake helpers
├── .github/            # GitHub Actions workflows
└── README.md
```

---

## Building the Source Code

This project uses **Conan** for dependency management and **CMake** for builds.

### Building for Release Mode

```bash
conan build . --build=missing
```

---

## Running the Server

Command to run the HTTP server implementation example:

```bash
# in release mode
./build/Release/server_impl_bin
```

---

## Running the Tests

Command to execute the automated test suite:

```bash
# run tests
./build/Release/server_tests_bin
```

---

## Code Coverage

Coverage reports are generated using **lcov**.

### Generate Coverage Reports

```bash
make -C build/Release coverage
```

### Open the Coverage Report

```bash
# specific to Linux
xdg-open build/Release/CMakeFiles/server_library.dir/src/out/index.html
```

---

## CI / GitHub Actions

The project includes a GitHub Actions workflow that automatically:

* Builds the project
* Runs the gtest suite
* Generates an lcov coverage report

### Coverage Report (GitHub Actions)

![lcov Coverage Report](docs/images/github-actions-lcov.png)

### Automated gtest Execution (GitHub Actions)

<img src="doc_images/gtest.png" alt="gtest CI Run" height="400">

---

## Project Management

Development tasks and progress are tracked using a **GitHub Projects Kanban board**.

<img src="doc_images/kanban.png" alt="github projects kanban board" height="400">

---

## Key Skills/Lessons Learned

* **Designing extensible routing systems** in C++ requires careful separation between route compilation and request
  matching
* **Understanding of TCP and port implementation on Linux**
* **Parsing of strings** (client requests) to get individual strings (HTTP method, route, request body)
* **Implementing Test driven development (TDD)** to improve confidence in changes and reduce regressions when refactoring
* **Setting up code coverage tools** (lcov) provide insight on what portions of the codebase are covered by testing
* **Setting up CI automation with GitHub Actions** to automate testing, code coverage reports, and enforce consistent quality for PRs
* **Better understanding of HTTP REST standards**, such as knowing which HTTP methods should have request bodies ignored


C++ specific lessons learned:
* **Using Conan** to simplify dependency management across environments
* **Learning CMake**
  * Creating functions in CMake to automatically generate code coverage reports
* **Implementing RAII** to improve memory safety and ensure automatic cleanup
* **Implementing more modern C++** practices:
  * std::threads, lambdas for route handlers, RAII-style object lifetime, ...
  * exception usage instead of C-style exits, thread sleeping (std::this_thread::sleep_for)
* Use `string_view` to avoid copying an entire `string` object

---

## Challenges:
* Implementing C++ practices while utilizing the Linux port library written in C
  * I had to encapsulate many parts of the C-specific library functions and employ RAII-style C++ objects (ex: closing ports before deconstruction)
* Designing a routing system that allows a programmer to easily map a function pointer to a route 
* Refactoring the routing system to allow for path parameters
