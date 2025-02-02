#include "pch.h"
#include "Shader.hpp"

#include "Renderer2D.hpp"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vulkan/VulkanShader.h"

#include <filesystem>

namespace Snowstorm
{
	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer2D::GetAPI())
		{
		case RendererAPI::API::None:
			SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::OpenGL:
			return CreateRef<OpenGLShader>(filepath);
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanShader>(filepath);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	void ShaderLibrarySingleton::Add(const Ref<Shader>& shader, const std::string& filepath)
	{
		SS_CORE_ASSERT(!Exists(filepath), "Shader already exists!");
		m_Shaders[filepath] = shader;
	}

	Ref<Shader> ShaderLibrarySingleton::Load(const std::string& filepath)
	{
		if (Exists(filepath))
		{
			return Get(filepath);
		}

		auto shader = Shader::Create(filepath);
		Add(shader, filepath);

		m_LastModifications[filepath] = std::filesystem::last_write_time(filepath);

		return shader;
	}

	Ref<Shader> ShaderLibrarySingleton::Get(const std::string& filepath)
	{
		SS_CORE_ASSERT(Exists(filepath), "Shader not found!");
		return m_Shaders[filepath];
	}

	bool ShaderLibrarySingleton::Exists(const std::string& filepath) const
	{
		return m_Shaders.contains(filepath);
	}

	void ShaderLibrarySingleton::ReloadAll()
	{
		for (const auto& [filepath, lastModified] : m_LastModifications)
		{
			if (std::filesystem::last_write_time(filepath) > lastModified)
			{
				Get(filepath)->Recompile();
			}
		}
	}
}
