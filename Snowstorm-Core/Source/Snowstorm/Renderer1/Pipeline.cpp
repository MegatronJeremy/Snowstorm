#include "pch.h"
#include "Pipeline.h"

#include "Renderer.h"

#include "Snowstorm/Platform/Vulkan/VulkanPipeline.h"

#include "Snowstorm/Renderer/RendererAPI.h"

namespace Snowstorm {

	Ref<Pipeline> Pipeline::Create(const PipelineSpecification& spec)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:    return nullptr;
			case RendererAPIType::Vulkan:  return Ref<VulkanPipeline>::Create(spec);
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}