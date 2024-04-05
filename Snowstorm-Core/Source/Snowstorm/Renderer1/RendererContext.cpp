#include "pch.h"
#include "RendererContext.h"

#include "Snowstorm/Renderer/RendererAPI.h"

#include "Snowstorm/Platform/OpenGL/OpenGLContext.h"

namespace Snowstorm {

	Ref<RendererContext> RendererContext::Create()
	{
		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:    return nullptr;
			case RendererAPIType::OpenGL:  return Ref<OpenGLContext>::Create();
		}
		SS_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

}