#include "UniformBuffer.hpp"

#include "Renderer2D.hpp"

#include "Platform/OpenGL/OpenGLUniformBuffer.hpp"

namespace Snowstorm
{
	std::shared_ptr<UniformBuffer> UniformBuffer::Create(uint32_t size, uint32_t binding)
	{
		switch (Renderer2D::GetAPI())
		{
		case RendererAPI::API::None:
			SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLUniformBuffer>(size, binding);
		case RendererAPI::API::Vulkan:
			SS_CORE_ASSERT(false, "VulkanUniformBuffer is not yet supported!");
			return nullptr;
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
