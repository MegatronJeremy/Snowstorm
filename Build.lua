include "Dependencies.lua"

-- premake5.lua
workspace "Snowstorm"
   architecture "x64"
   configurations { "Debug", "Release", "Dist" }
   startproject "Snowstorm-App"

   -- Workspace-wide build options for MSVC
   filter "system:windows"
      buildoptions { "/EHsc", "/Zc:preprocessor", "/Zc:__cplusplus" }

OutputDir = "%{cfg.system}-%{cfg.architecture}/%{cfg.buildcfg}"

group "Dependencies"
   include "Snowstorm-Core/Vendor/GLFW/Build-GLFW.lua"
group ""

group "Core"
	include "Snowstorm-Core/Build-Core.lua"
group ""

include "Snowstorm-App/Build-App.lua"