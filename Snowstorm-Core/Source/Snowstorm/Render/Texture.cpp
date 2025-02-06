#include "pch.h"
#include "Texture.hpp"

#include "Renderer2D.hpp"
#include "Platform/OpenGL/OpenGLTexture.h"
#include "Platform/Vulkan/VulkanTexture.h"

namespace Snowstorm
{
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Renderer2D::GetAPI())
		{
		case RendererAPI::API::None:
			SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(width, height);
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanTexture2D>(width, height);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer2D::GetAPI())
		{
		case RendererAPI::API::None:
			SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLTexture2D>(path);
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanTexture2D>(path);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
