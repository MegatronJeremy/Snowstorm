#include "pch.h"
#include "Buffer.h"

#include "Renderer.h"

#include "Platform/Vulkan/VulkanBuffer.h"

namespace Snowstorm
{
	Ref<VertexBuffer> VertexBuffer::Create(uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanVertexBuffer>(size);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<VertexBuffer> VertexBuffer::Create(const float* vertices, const uint32_t size)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanVertexBuffer>(vertices, size);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(const uint32_t* indices, const uint32_t count)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanIndexBuffer>(indices, count);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
