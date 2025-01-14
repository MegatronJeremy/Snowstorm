# Snowstorm-App CMake Configuration
add_executable(Snowstorm-App)

# Set the C++ standard
set_target_properties(Snowstorm-App PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# Add source files
file(GLOB_RECURSE APP_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)
target_sources(Snowstorm-App PRIVATE ${APP_SOURCES})

# Include directories
target_include_directories(Snowstorm-App PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
    ${CMAKE_SOURCE_DIR}/Snowstorm-Core/Source
    ${CMAKE_SOURCE_DIR}/Vendor/glm
    ${CMAKE_SOURCE_DIR}/Vendor/entt/include
    ${CMAKE_SOURCE_DIR}/Vendor/ImGui
    ${CMAKE_SOURCE_DIR}/Vendor/spdlog/include
)

# Link the Snowstorm-Core library
target_link_libraries(Snowstorm-App PRIVATE Snowstorm-Core)

# Output directories
set_target_properties(Snowstorm-App PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>/Snowstorm-App
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>/Snowstorm-App
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>/Snowstorm-App
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(Snowstorm-App PRIVATE WINDOWS)
    target_compile_options(Snowstorm-App PRIVATE /EHsc)
endif()

# Configuration-specific settings
target_compile_definitions(Snowstorm-App PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:RELEASE>
    $<$<CONFIG:Dist>:DIST>
)

# Runtime library options
target_compile_options(Snowstorm-App PRIVATE
    $<$<CONFIG:Debug>:/MDd> # Dynamic Debug runtime
    $<$<CONFIG:Release>:/MD> # Dynamic Release runtime
    $<$<CONFIG:Dist>:/MD>    # Dynamic Distribution runtime
)

# Optimization flags for Release and Dist
target_compile_options(Snowstorm-App PRIVATE
    $<$<CONFIG:Release>:/O2>
    $<$<CONFIG:Dist>:/O2>
)

# Symbols for Debug builds
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    target_compile_options(Snowstorm-App PRIVATE /Z7)
endif()
