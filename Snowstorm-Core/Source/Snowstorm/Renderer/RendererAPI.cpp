#include "pch.h"
#include "Snowstorm/Renderer/RendererAPI.h"

#include "Platform/Vulkan/VulkanRendererAPI.h"

namespace Snowstorm
{
	RendererAPI::API RendererAPI::s_API = API::Vulkan;

	Scope<RendererAPI> RendererAPI::Create()
	{
		switch (s_API)
		{
		case API::None: SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case API::Vulkan: return CreateScope<VulkanRendererAPI>();
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
