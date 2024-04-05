#include "pch.h"
#include "RenderCommandBuffer.h"

#include "Snowstorm/Platform/Vulkan/VulkanRenderCommandBuffer.h"

#include "Snowstorm/Renderer/RendererAPI.h"

namespace Snowstorm {

	Ref<RenderCommandBuffer> RenderCommandBuffer::Create(uint32_t count, const std::string& debugName)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:    return nullptr;
			case RendererAPIType::Vulkan:  return Ref<VulkanRenderCommandBuffer>::Create(count, debugName);
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<RenderCommandBuffer> RenderCommandBuffer::CreateFromSwapChain(const std::string& debugName)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:    return nullptr;
			case RendererAPIType::Vulkan:  return Ref<VulkanRenderCommandBuffer>::Create(debugName, true);
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}