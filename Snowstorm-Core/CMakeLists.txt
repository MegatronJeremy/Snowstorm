cmake_minimum_required(VERSION 3.15)
project(Snowstorm-Core VERSION 1.0 LANGUAGES CXX)

add_library(Snowstorm-Core STATIC)

set_target_properties(Snowstorm-Core PROPERTIES
    CXX_STANDARD 20
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

file(GLOB_RECURSE CORE_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)
target_sources(Snowstorm-Core PRIVATE ${CORE_SOURCES})

target_include_directories(Snowstorm-Core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Source
)

find_package(spdlog REQUIRED CONFIG)
find_package(fmt REQUIRED CONFIG)
find_package(stb REQUIRED CONFIG)
find_package(glm REQUIRED CONFIG)
find_package(EnTT REQUIRED CONFIG)
find_package(glfw3 REQUIRED CONFIG)
find_package(glew REQUIRED CONFIG)
find_package(imgui REQUIRED CONFIG)
find_package(VulkanLoader REQUIRED CONFIG)
find_package(VulkanHeaders REQUIRED CONFIG)
find_package(vulkan-validationlayers REQUIRED CONFIG)

target_link_libraries(Snowstorm-Core PUBLIC
    spdlog::spdlog
    fmt::fmt
    stb::stb
    glm::glm
    EnTT::EnTT
    glfw
    GLEW::GLEW
    imgui::imgui
    Vulkan::Loader
    vulkan-headers::vulkan-headers
    vulkan-validationlayers::vulkan-validationlayers
    opengl32
)
