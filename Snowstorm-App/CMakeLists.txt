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

# Copy assets to build directory
add_custom_command(TARGET Snowstorm-App
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/Assets
    ${CMAKE_CURRENT_BINARY_DIR}/Assets
)