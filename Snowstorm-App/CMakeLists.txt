# Snowstorm-App CMake Configuration
add_executable(Snowstorm-App)

# Set C++ standard
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
)

# Link libraries
target_link_libraries(Snowstorm-App PRIVATE
    Snowstorm-Core  # Link the core library
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
set_target_properties(Snowstorm-App PROPERTIES
    MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>"
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
