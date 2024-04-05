#include "pch.h"
#include "Material.h"

#include "Snowstorm/Platform/Vulkan/VulkanMaterial.h"

#include "Snowstorm/Renderer/RendererAPI.h"

namespace Snowstorm {

	Ref<Material> Material::Create(const Ref<Shader>& shader, const std::string& name)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::Vulkan: return Ref<VulkanMaterial>::Create(shader, name);
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
	
	Ref<Material> Material::Copy(const Ref<Material>& other, const std::string& name)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::Vulkan: return Ref<VulkanMaterial>::Create(other, name);
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}