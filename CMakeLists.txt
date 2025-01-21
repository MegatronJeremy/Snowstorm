cmake_minimum_required(VERSION 3.16)
project(Snowstorm LANGUAGES CXX)

# Log CMake version and project details
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "Project name: ${PROJECT_NAME}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")

# Global settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "Global C++ standard settings applied.")

# Add configurations for Debug, Release, and Dist
if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Dist" CACHE STRING "" FORCE)
    message(STATUS "Build configurations: ${CMAKE_CONFIGURATION_TYPES}")
endif()

# Define flags for custom build types
set(CMAKE_EXE_LINKER_FLAGS_DIST "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
set(CMAKE_SHARED_LINKER_FLAGS_DIST "${CMAKE_SHARED_LINKER_FLAGS_RELEASE}")
set(CMAKE_MODULE_LINKER_FLAGS_DIST "${CMAKE_MODULE_LINKER_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_DIST "${CMAKE_CXX_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_DIST "${CMAKE_C_FLAGS_RELEASE}")

message(STATUS "Custom build type 'Dist' configured with Release flags.")

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>)

message(STATUS "Output directories set:")
message(STATUS "  Archive: ${CMAKE_ARCHIVE_OUTPUT_DIRECTORY}")
message(STATUS "  Library: ${CMAKE_LIBRARY_OUTPUT_DIRECTORY}")
message(STATUS "  Runtime: ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

# Add subdirectories for core and app
message(STATUS "Adding subdirectories for core and app.")
add_subdirectory(src/Snowstorm-Core)
add_subdirectory(src/Snowstorm-App)

# Set the default startup project for Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "Snowstorm-App")
message(STATUS "Default startup project set to: Snowstorm-App")

# Log completion of root CMake configuration
message(STATUS "Root CMakeLists.txt configuration completed.")
