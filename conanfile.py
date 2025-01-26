from conan import ConanFile
from conan.tools.cmake import CMake, cmake_layout
from conan.tools.files import copy
import os
import subprocess

class SnowstormConan(ConanFile):
    name = "snowstorm"
    version = "0.1"
    settings = "os", "compiler", "build_type", "arch"
    generators = "CMakeDeps", "CMakeToolchain"

    requires = [
        "vulkan-headers/1.3.239.0",
        "vulkan-loader/1.3.239.0",
        "vulkan-validationlayers/1.3.239.0",
        "glfw/3.4",
        "glew/2.2.0",
        "glm/cci.20230113",
        "spdlog/1.15.0",
        "imgui/cci.20230105+1.89.2.docking",
        "fmt/11.0.2",
        "stb/cci.20240531",
        "entt/3.14.0"
    ]
           
    def generate(self):        
        # Copy ImGui backends to project
        copy(self, "*glfw*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "Snowstorm-Core", "Source", "Platform", "OpenGL", "backends"))
        copy(self, "*opengl3*", os.path.join(self.dependencies["imgui"].package_folder,
            "res", "bindings"), os.path.join(self.source_folder, "Snowstorm-Core", "Source", "Platform", "OpenGL", "backends"))
  
    def layout(self):
        cmake_layout(self)