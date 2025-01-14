# Snowstorm-Core CMake Configuration
add_library(Snowstorm-Core STATIC)

# Set the C++ standard
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
    "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/stb_image/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/stb_image/*.h"
    "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glm/glm/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glm/glm/*.inl"
)
target_sources(Snowstorm-Core PRIVATE ${CORE_SOURCES})

# Include directories
target_include_directories(Snowstorm-Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/spdlog/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/GLFW/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/glm
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/Glad/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/ImGui
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/stb_image
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/entt/include
    ${CMAKE_CURRENT_SOURCE_DIR}/Vendor/Vulkan-Headers/include
)

# Link libraries
target_link_libraries(Snowstorm-Core PUBLIC
    glfw
    Glad
    ImGui
    vulkan
    opengl32
)

# Output directories
set_target_properties(Snowstorm-Core PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>/Snowstorm-Core
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>/Snowstorm-Core
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>/Snowstorm-Core
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
target_compile_options(Snowstorm-Core PRIVATE
    $<$<CONFIG:Debug>:/MDd> # Dynamic Debug runtime
    $<$<CONFIG:Release>:/MD> # Dynamic Release runtime
    $<$<CONFIG:Dist>:/MD>    # Dynamic Distribution runtime
)

# Optimization flags
target_compile_options(Snowstorm-Core PRIVATE
    $<$<CONFIG:Release>:/O2>
    $<$<CONFIG:Dist>:/O2>
)

# Symbols for Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(Snowstorm-Core PRIVATE /Z7)
endif()
