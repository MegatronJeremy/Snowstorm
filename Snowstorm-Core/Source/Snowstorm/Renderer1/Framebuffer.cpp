#include "pch.h"
#include "Framebuffer.h"

#include "Snowstorm/Platform/Vulkan/VulkanFramebuffer.h"

#include "Snowstorm/Renderer/RendererAPI.h"

namespace Snowstorm {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		Ref<Framebuffer> result = nullptr;

		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:		return nullptr;
			case RendererAPIType::Vulkan:	result = Ref<VulkanFramebuffer>::Create(spec); break;
		}
		return result;
	}

}
