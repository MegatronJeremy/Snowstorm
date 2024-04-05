#include "pch.h"
#include "UniformBufferSet.h"

#include "Snowstorm/Renderer/Renderer.h"

#include "Snowstorm/Platform/OpenGL/OpenGLUniformBufferSet.h"

#include "Snowstorm/Renderer/RendererAPI.h"

namespace Snowstorm {

	Ref<UniformBufferSet> UniformBufferSet::Create(uint32_t size, uint32_t framesInFlight)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:   return nullptr;
			case RendererAPIType::OpenGL: return Ref<OpenGLUniformBufferSet>::Create(size, framesInFlight);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}