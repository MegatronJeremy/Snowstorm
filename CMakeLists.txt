cmake_minimum_required(VERSION 3.16)
project(Snowstorm LANGUAGES CXX)

# Include the Conan toolchain file
include(${CMAKE_BINARY_DIR}/conan_toolchain.cmake OPTIONAL RESULT_VARIABLE CONAN_TOOLCHAIN_INCLUDED)
if(NOT CONAN_TOOLCHAIN_INCLUDED)
    message(FATAL_ERROR "Conan toolchain not found. Did you run `conan install`?")
endif()

# Global settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Add configurations for Debug, Release, and Dist
if(CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Dist" CACHE STRING "" FORCE)
endif()

# Set output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Binaries/$<CONFIG>)

# Add subdirectories for core and app
add_subdirectory(Snowstorm-Core)
add_subdirectory(Snowstorm-App)

# Set the default startup project for Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "Snowstorm-App")
