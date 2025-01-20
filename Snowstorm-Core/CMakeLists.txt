# Snowstorm-Core CMake Configuration
add_library(Snowstorm-Core STATIC)

# Set C++ standard
set_target_properties(Snowstorm-Core PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Precompiled header
target_precompile_headers(Snowstorm-Core PRIVATE Source/pch.cpp)

# Add source files
file(GLOB_RECURSE CORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)
target_sources(Snowstorm-Core PRIVATE ${CORE_SOURCES})

# Include directories
target_include_directories(Snowstorm-Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

# Link libraries from Conan
find_package(spdlog REQUIRED CONFIG)
find_package(glm REQUIRED CONFIG)
find_package(glfw3 REQUIRED CONFIG)
find_package(imgui REQUIRED CONFIG)
find_package(VulkanLoader REQUIRED CONFIG)
find_package(VulkanHeaders REQUIRED CONFIG)

target_link_libraries(Snowstorm-Core PUBLIC
    spdlog::spdlog_header_only
    glfw                       # From Conan
    glm::glm                   # From Conan
    imgui::imgui               # From Conan
    Vulkan::Loader             # From Conan
    vulkan-headers::vulkan-headers # From Conan
    opengl32                   # Windows-specific
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(Snowstorm-Core PRIVATE WINDOWS)
    target_compile_options(Snowstorm-Core PRIVATE /EHsc)
endif()

# Configuration-specific settings
target_compile_definitions(Snowstorm-Core PRIVATE
    $<$<CONFIG:Debug>:SS_DEBUG>
    $<$<CONFIG:Release>:SS_RELEASE>
    $<$<CONFIG:Dist>:SS_DIST>
)

# Runtime library options
set_target_properties(Snowstorm-Core PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
)

# Output directories
set_target_properties(Snowstorm-Core PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>/Snowstorm-Core
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>/Snowstorm-Core
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>/Snowstorm-Core
)
