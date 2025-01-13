project "Snowstorm-Core"
kind "StaticLib"
language "C++"
cppdialect "C++20"
targetdir "Binaries/%{cfg.buildcfg}"
staticruntime "off"

pchheader "pch.h"
pchsource "Source/pch.cpp"

files {"Source/**.h", "Source/**.cpp", "Vendor/stb_image/**.h", "Vendor/stb_image/**.cpp",
             "Vendor/glm/glm/**.hpp", "Vendor/glm/glm/**.inl"}

includedirs {"Source", "%{IncludeDir.spdlog}", "%{IncludeDir.GLFW}", "%{IncludeDir.glm}", "%{IncludeDir.Glad}",
             "%{IncludeDir.ImGui}", "%{IncludeDir.stb_image}", "%{IncludeDir.entt}", "%{IncludeDir.VulkanHeaders}"}

links {"GLFW", "Glad", "ImGui", "opengl32.lib", "Vulkan-Loader"}

targetdir("../Binaries/" .. OutputDir .. "/%{prj.name}")
objdir("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

filter "system:windows"
systemversion "latest"
defines {}

filter "configurations:Debug"
defines {"SS_DEBUG"}
runtime "Debug"
symbols "On"

filter "configurations:Release"
defines {"SS_RELEASE"}
runtime "Release"
optimize "On"
symbols "On"

filter "configurations:Dist"
defines {"SS_DIST"}
runtime "Release"
optimize "On"
symbols "Off"
