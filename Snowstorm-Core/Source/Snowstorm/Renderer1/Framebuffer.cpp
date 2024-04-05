#include "pch.h"
#include "Framebuffer.h"

#include "Snowstorm/Platform/OpenGL/OpenGLFramebuffer.h"

#include "Snowstorm/Renderer/RendererAPI.h"

namespace Snowstorm {

	Ref<Framebuffer> Framebuffer::Create(const FramebufferSpecification& spec)
	{
		Ref<Framebuffer> result = nullptr;

		switch (RendererAPI::Current())
		{
			case RendererAPIType::None:		return nullptr;
			case RendererAPIType::OpenGL:	result = Ref<OpenGLFramebuffer>::Create(spec); break;
		}
		return result;
	}

}
