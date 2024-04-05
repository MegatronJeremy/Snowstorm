#include "pch.h"
#include "VertexBuffer.h"

#include "Renderer.h"

#include "Snowstorm/Platform/Vulkan/VulkanVertexBuffer.h"

#include "Snowstorm/Renderer/RendererAPI.h"

namespace Snowstorm {

	Ref<VertexBuffer> VertexBuffer::Create(void* data, uint64_t size, VertexBufferUsage usage)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:    return nullptr;
			case RendererAPIType::Vulkan:  return Ref<VulkanVertexBuffer>::Create(data, size, usage);
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(uint64_t size, VertexBufferUsage usage)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:    return nullptr;
			case RendererAPIType::Vulkan:  return Ref<VulkanVertexBuffer>::Create(size, usage);
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}
