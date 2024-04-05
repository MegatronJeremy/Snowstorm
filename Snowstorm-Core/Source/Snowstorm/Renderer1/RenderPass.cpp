#include "pch.h"
#include "RenderPass.h"

#include "Renderer.h"

#include "Snowstorm/Platform/Vulkan/VulkanRenderPass.h"

#include "Snowstorm/Renderer/RendererAPI.h"

namespace Snowstorm {

	Ref<RenderPass> RenderPass::Create(const RenderPassSpecification& spec)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:    SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPIType::Vulkan:  return Ref<VulkanRenderPass>::Create(spec);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}