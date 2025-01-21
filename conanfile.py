from conan import ConanFile
from conan.tools.cmake import CMake, CMakeDeps, cmake_layout


class SnowstormConan(ConanFile):
    name = "snowstorm"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    requires = [
        "vulkan-headers/1.3.290.0",
        "vulkan-loader/1.3.290.0",
        "glfw/3.4",
        "glm/cci.20230113",  # Any patch update of 0.9.9.x
        "spdlog/1.15.0",
        "imgui/cci.20230105+1.89.2.docking",
        "fmt/11.0.2",
        "opengl/system"
    ]

    def layout(self):
        cmake_layout(self)
