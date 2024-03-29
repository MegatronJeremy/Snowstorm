project "Snowstorm-Core"
   kind "StaticLib"
   language "C++"
   cppdialect "C++20"
   targetdir "Binaries/%{cfg.buildcfg}"
   staticruntime "off"

   pchheader "pch.h"
   pchsource "Source/pch.cpp"

   files 
   { 
    "Source/**.h", 
    "Source/**.cpp", 

    "Vendor/glm/glm/**.hpp",
    "Vendor/glm/glm/**.inl"
   }

   includedirs
   {
    "Source",
    "Vendor/spdlog/include",
    "%{IncludeDir.GLFW}",
    "%{IncludeDir.glm}",
    "%{IncludeDir.VulkanSDK}"
   }

   links
   {
    "GLFW",
    "%{Library.Vulkan}"
   }

   targetdir ("../Binaries/" .. OutputDir .. "/%{prj.name}")
   objdir ("../Binaries/Intermediates/" .. OutputDir .. "/%{prj.name}")

   filter "system:windows"
       systemversion "latest"
       defines { }

   filter "configurations:Debug"
       defines { "SS_DEBUG" }
       runtime "Debug"
       symbols "On"

       links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}

   filter "configurations:Release"
       defines { "SS_RELEASE" }
       runtime "Release"
       optimize "On"
       symbols "On"

       links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}

   filter "configurations:Dist"
       defines { "SS_DIST" }
       runtime "Release"
       optimize "On"
       symbols "Off"

       links
		{
			"%{Library.ShaderC_Release}",
			"%{Library.SPIRV_Cross_Release}",
			"%{Library.SPIRV_Cross_GLSL_Release}"
		}