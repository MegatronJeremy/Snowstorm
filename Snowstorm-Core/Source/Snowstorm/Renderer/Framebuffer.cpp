#include "pch.h"
#include "Framebuffer.hpp"

#include "Snowstorm/Renderer/Renderer2D.hpp"

#include "Platform/OpenGL/OpenGLFramebuffer.h"
#include "Platform/Vulkan/VulkanFramebuffer.h"

namespace Snowstorm
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer2D::GetAPI())
		{
		case RendererAPI::API::None:
			SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLFrameBuffer>(spec);
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanFrameBuffer>(spec);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
