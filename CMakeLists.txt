cmake_minimum_required(VERSION 3.16)
project(Snowstorm LANGUAGES CXX)

# Global settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Dist" CACHE STRING "" FORCE)

# Set architecture and output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>)

# Include directories
set(SPDLOG_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Snowstorm-Core/Vendor/spdlog/include)
set(GLFW_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Snowstorm-Core/Vendor/GLFW/include)
set(GLAD_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Snowstorm-Core/Vendor/Glad/include)
set(GLM_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Snowstorm-Core/Vendor/glm)
set(IMGUI_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Snowstorm-Core/Vendor/imgui)
set(STB_IMAGE_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Snowstorm-Core/Vendor/stb_image)
set(ENTT_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Snowstorm-Core/Vendor/entt/include)
set(VULKAN_HEADERS_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/Snowstorm-Core/Vendor/Vulkan-Headers/include)

# Library directories
set(VULKAN_LIB_DIR ${VULKAN_SDK}/Lib)

# Vulkan libraries
set(VULKAN_LIB ${VULKAN_LIB_DIR}/vulkan-1.lib)
set(VULKAN_UTILS_LIB ${VULKAN_LIB_DIR}/VkLayer_utils.lib)

set(SHADERC_DEBUG_LIB ${VULKAN_LIB_DIR}/shaderc_sharedd.lib)
set(SPIRV_CROSS_DEBUG_LIB ${VULKAN_LIB_DIR}/spirv-cross-cored.lib)
set(SPIRV_CROSS_GLSL_DEBUG_LIB ${VULKAN_LIB_DIR}/spirv-cross-glsld.lib)
set(SPIRV_TOOLS_DEBUG_LIB ${VULKAN_LIB_DIR}/SPIRV-Toolsd.lib)

set(SHADERC_RELEASE_LIB ${VULKAN_LIB_DIR}/shaderc_shared.lib)
set(SPIRV_CROSS_RELEASE_LIB ${VULKAN_LIB_DIR}/spirv-cross-core.lib)
set(SPIRV_CROSS_GLSL_RELEASE_LIB ${VULKAN_LIB_DIR}/spirv-cross-glsl.lib)

# Windows system libraries
set(WINSOCK_LIB Ws2_32.lib)
set(WINMM_LIB Winmm.lib)
set(WINVERSION_LIB Version.lib)
set(BCRYPT_LIB Bcrypt.lib)

# Vulkan SDK
set(VULKAN_SDK $ENV{VULKAN_SDK})

# Include dependencies
add_subdirectory(Snowstorm-Core/Vendor/GLFW)
add_subdirectory(Snowstorm-Core/Vendor/Glad)
add_subdirectory(Snowstorm-Core/Vendor/spdlog)
add_subdirectory(Snowstorm-Core/Vendor/imgui)
add_subdirectory(Snowstorm-Core/Vendor/Vulkan-Loader)
add_subdirectory(Snowstorm-Core/Vendor/Vulkan-Headers)

# Include main projects
add_subdirectory(Snowstorm-App)
add_subdirectory(Snowstorm-Core)
add_subdirectory(Snowstorm-Editor)

# Set the default startup project (optional for IDEs like Visual Studio)
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "Snowstorm-Editor")