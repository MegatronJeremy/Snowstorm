#include "pch.h"
#include "Framebuffer.h"

#include "Snowstorm/Renderer/Renderer.h"

#include "Platform/Vulkan/VulkanFramebuffer.h"

namespace Snowstorm
{
	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanFrameBuffer>(spec);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
