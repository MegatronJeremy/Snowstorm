#include "pch.h"
#include "ComputePass.h"

#include "Renderer.h"

#include "Snowstorm/Platform/Vulkan/VulkanComputePass.h"

#include "Snowstorm/Renderer/RendererAPI.h"

namespace Snowstorm {

	Ref<ComputePass> ComputePass::Create(const ComputePassSpecification& spec)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:    SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPIType::Vulkan:  return Ref<VulkanComputePass>::Create(spec);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
