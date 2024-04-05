#include "pch.h"
#include "PipelineCompute.h"

#include "Snowstorm/Renderer/RendererAPI.h"
#include "Snowstorm/Platform/Vulkan/VulkanComputePipeline.h"

namespace Snowstorm {

	Ref<PipelineCompute> PipelineCompute::Create(Ref<Shader> computeShader)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::Vulkan: return Ref<VulkanComputePipeline>::Create(computeShader);
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}