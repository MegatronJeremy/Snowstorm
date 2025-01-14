# Glad CMake Configuration
add_library(Glad STATIC)

# Set C as the programming language for this target
set_target_properties(Glad PROPERTIES
    LANGUAGE C
    C_STANDARD 11
)

# Add source files
target_sources(Glad PRIVATE
    include/glad/glad.h
    include/KHR/khrplatform.h
    src/glad.c
)

# Include directories
target_include_directories(Glad PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

# Output directories
set_target_properties(Glad PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>/Glad
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>/Glad
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>/Glad
)

# Platform-specific settings
if(WIN32)
    target_compile_definitions(Glad PRIVATE WINDOWS)
endif()

# Configuration-specific settings
target_compile_options(Glad PRIVATE
    $<$<CONFIG:Debug>:-g>
    $<$<CONFIG:Release>:-O2>
    $<$<CONFIG:Dist>:-O2>
)

target_compile_definitions(Glad PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:NDEBUG>
    $<$<CONFIG:Dist>:NDEBUG>
)

# Disable warnings for this library
if(MSVC)
    target_compile_options(Glad PRIVATE /w)
else()
    target_compile_options(Glad PRIVATE -w)
endif()
