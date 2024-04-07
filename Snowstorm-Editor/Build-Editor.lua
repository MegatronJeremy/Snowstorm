project "Snowstorm-Editor"
kind "ConsoleApp"
language "C++"
cppdialect "C++20"
targetdir "Binaries/%{cfg.buildcfg}"
staticruntime "off"

files {"Source/**.h", "Source/**.cpp"}

includedirs {"Source", "%{IncludeDir.spdlog}", "%{wks.location}/Snowstorm-Core/Source", "%{IncludeDir.ImGui}",
             "%{IncludeDir.glm}", "%{IncludeDir.entt}"}
             
targetdir("../Binaries/" .. OutputDir .. "/%{prj.name}")
objdir("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

links {"Snowstorm-Core"}

filter "system:windows"
systemversion "latest"
defines {"WINDOWS"}

filter "configurations:Debug"
defines {"DEBUG"}
runtime "Debug"
symbols "on"

filter "configurations:Release"
defines {"RELEASE"}
runtime "Release"
optimize "on"
symbols "On"

filter "configurations:Dist"
defines {"DIST"}
runtime "Release"
optimize "on"
symbols "Off"

