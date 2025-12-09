from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout


class ServerRecipe(ConanFile):
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeToolchain", "CMakeDeps"
    options = {"test": [True, False]}
    default_options = {"test": False}

    def requirements(self):
        if self.options.test:
            self.requires("gtest/1.17.0")

    def build(self):
        cmake = CMake(self)
        defs = {
            "ENABLE_TESTS": "ON" if self.options.test else "OFF"
        }
        cmake.configure(variables=defs)
        cmake.build()

    def layout(self):
        cmake_layout(self)
