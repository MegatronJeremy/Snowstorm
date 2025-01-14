# Snowstorm-Editor CMake Configuration
add_executable(Snowstorm-Editor)

# Set the C++ standard
set_target_properties(Snowstorm-Editor PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Add source files
file(GLOB_RECURSE EDITOR_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)
target_sources(Snowstorm-Editor PRIVATE ${EDITOR_SOURCES})

# Include directories
target_include_directories(Snowstorm-Editor PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Snowstorm-Core/Source
    ${CMAKE_SOURCE_DIR}/Vendor/spdlog/include
    ${CMAKE_SOURCE_DIR}/Vendor/ImGui
    ${CMAKE_SOURCE_DIR}/Vendor/glm
    ${CMAKE_SOURCE_DIR}/Vendor/entt/include
)

# Link libraries
target_link_libraries(Snowstorm-Editor PRIVATE Snowstorm-Core)

# Output directories
set_target_properties(Snowstorm-Editor PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>/Snowstorm-Editor
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>/Snowstorm-Editor
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>/Snowstorm-Editor
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(Snowstorm-Editor PRIVATE WINDOWS)
    target_compile_options(Snowstorm-Editor PRIVATE /EHsc)
endif()

# Configuration-specific settings
target_compile_definitions(Snowstorm-Editor PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>
    $<$<CONFIG:Dist>:DIST>
)

# Runtime library options
target_compile_options(Snowstorm-Editor PRIVATE
    $<$<CONFIG:Debug>:/MDd> # Dynamic Debug runtime
    $<$<CONFIG:Release>:/MD> # Dynamic Release runtime
    $<$<CONFIG:Dist>:/MD>    # Dynamic Distribution runtime
)

# Optimization flags
target_compile_options(Snowstorm-Editor PRIVATE
    $<$<CONFIG:Release>:/O2>
    $<$<CONFIG:Dist>:/O2>
)

# Symbols for Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(Snowstorm-Editor PRIVATE /Z7)
endif()
