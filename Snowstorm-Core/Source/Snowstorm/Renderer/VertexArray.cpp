#include "pch.h"
#include "VertexArray.h"

#include "Renderer.h"
#include "Platform/Vulkan/VulkanVertexArray.h"

namespace Snowstorm
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanVertexArray>();
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
