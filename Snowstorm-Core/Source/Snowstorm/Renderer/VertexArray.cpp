#include "pch.h"
#include "VertexArray.hpp"

#include "Renderer2D.hpp"
#include "Platform/OpenGL/OpenGLVertexArray.h"
#include "Platform/Vulkan/VulkanVertexArray.h"

namespace Snowstorm
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer2D::GetAPI())
		{
		case RendererAPI::API::None:
			SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLVertexArray>();
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanVertexArray>();
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
