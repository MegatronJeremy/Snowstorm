cmake_minimum_required(VERSION 3.16)
project(Snowstorm LANGUAGES CXX)

# Global settings
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Organize predefined targets into a folder
set(PREDEFINED_TARGETS_FOLDER "Predefined")

# Add subdirectories for core and app
add_subdirectory(Snowstorm-Core)
add_subdirectory(Snowstorm-App)

# Set the default startup project for Visual Studio
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT "Snowstorm-App")
