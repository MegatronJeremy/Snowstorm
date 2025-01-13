-- Snowstorm Dependencies

VULKAN_SDK = os.getenv("VULKAN_SDK")

IncludeDir = {}
IncludeDir["spdlog"] = "%{wks.location}/Snowstorm-Core/Vendor/spdlog/include"
IncludeDir["GLFW"] = "%{wks.location}/Snowstorm-Core/Vendor/GLFW/include"
IncludeDir["Glad"] = "%{wks.location}/Snowstorm-Core/Vendor/Glad/include"
IncludeDir["glm"] = "%{wks.location}/Snowstorm-Core/Vendor/glm"
IncludeDir["ImGui"] = "%{wks.location}/Snowstorm-Core/Vendor/imgui"
IncludeDir["stb_image"] = "%{wks.location}/Snowstorm-Core/Vendor/stb_image"
IncludeDir["entt"] = "%{wks.location}/Snowstorm-Core/Vendor/entt/include"
IncludeDir["VulkanHeaders"] = "%{wks.location}/Snowstorm-Core/Vendor/Vulkan-Headers/include"

LibraryDir = {}

LibraryDir["VulkanSDK"] = "%{VULKAN_SDK}/Lib"

Library = {}

Library["Vulkan"] = "%{LibraryDir.VulkanSDK}/vulkan-1.lib"
Library["VulkanUtils"] = "%{LibraryDir.VulkanSDK}/VkLayer_utils.lib"

Library["ShaderC_Debug"] = "%{LibraryDir.VulkanSDK}/shaderc_sharedd.lib"
Library["SPIRV_Cross_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-cored.lib"
Library["SPIRV_Cross_GLSL_Debug"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsld.lib"
Library["SPIRV_Tools_Debug"] = "%{LibraryDir.VulkanSDK}/SPIRV-Toolsd.lib"

Library["ShaderC_Release"] = "%{LibraryDir.VulkanSDK}/shaderc_shared.lib"
Library["SPIRV_Cross_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-core.lib"
Library["SPIRV_Cross_GLSL_Release"] = "%{LibraryDir.VulkanSDK}/spirv-cross-glsl.lib"

-- Windows
Library["WinSock"] = "Ws2_32.lib"
Library["WinMM"] = "Winmm.lib"
Library["WinVersion"] = "Version.lib"
Library["BCrypt"] = "Bcrypt.lib"