#include "pch.h"
#include "IndexBuffer.h"

#include "Renderer.h"

#include "Snowstorm/Platform/Vulkan/VulkanIndexBuffer.h"

#include "Snowstorm/Renderer/RendererAPI.h"

namespace Snowstorm {

	Ref<IndexBuffer> IndexBuffer::Create(uint64_t size)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:    return nullptr;
			case RendererAPIType::Vulkan:  return Ref<VulkanIndexBuffer>::Create(size);
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(void* data, uint64_t size)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:    return nullptr;
			case RendererAPIType::Vulkan:  return Ref<VulkanIndexBuffer>::Create(data, size);
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}
