from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps


class SnowstormConan(ConanFile):
    name = "snowstorm"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    requires = [
        "vulkan-loader/1.3.243.0",
        "vulkan-headers/1.3.243.0",
        "spdlog/1.11.0",
        "glfw/3.3.8",
        "glm/0.9.9.8",
        "imgui/1.89.4"
    ]
    generators = "CMakeDeps", "CMakeToolchain"
    default_options = {
        "spdlog/*:header_only": True
    }

    def layout(self):
        self.folders.source = "."
        self.folders.build = "build"
