#include "pch.h"
#include "UniformBuffer.h"

#include "Snowstorm/Renderer/Renderer.h"

#include "Snowstorm/Platform/Vulkan/VulkanUniformBuffer.h"

#include "Snowstorm/Renderer/RendererAPI.h"

namespace Snowstorm {

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:     return nullptr;
			case RendererAPIType::Vulkan:  return Ref<VulkanUniformBuffer>::Create(size);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}