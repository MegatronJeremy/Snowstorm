#include "pch.h"
#include "RenderCommand.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Snowstorm
{
	RendererAPI* RenderCommand::s_RendererAPI = new VulkanRendererAPI;
}
