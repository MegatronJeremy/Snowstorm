#include "pch.h"
#include "PipelineCompute.h"

#include "Snowstorm/Renderer/RendererAPI.h"
#include "Snowstorm/Platform/OpenGL/OpenGLComputePipeline.h"

namespace Snowstorm {

	Ref<PipelineCompute> PipelineCompute::Create(Ref<Shader> computeShader)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::OpenGL: return Ref<OpenGLComputePipeline>::Create(computeShader);
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}