#include "pch.h"
#include "UniformBuffer.h"

#include "Snowstorm/Renderer/Renderer.h"

#include "Snowstorm/Platform/OpenGL/OpenGLUniformBuffer.h"

#include "Snowstorm/Renderer/RendererAPI.h"

namespace Snowstorm {

	Ref<UniformBuffer> UniformBuffer::Create(uint32_t size)
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:     return nullptr;
			case RendererAPIType::OpenGL:  return Ref<OpenGLUniformBuffer>::Create(size);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}
