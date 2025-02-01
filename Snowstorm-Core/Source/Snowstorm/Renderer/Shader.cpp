#include "pch.h"
#include "Shader.hpp"

#include "Renderer.hpp"
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/Vulkan/VulkanShader.h"

#include <filesystem>

namespace Snowstorm
{
	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
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

	void ShaderLibrarySingleton::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		SS_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	Ref<Shader> ShaderLibrarySingleton::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);

		m_LastModifications[filepath] = std::filesystem::last_write_time(filepath);

		return shader;
	}

	Ref<Shader> ShaderLibrarySingleton::Get(const std::string& name)
	{
		SS_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrarySingleton::Exists(const std::string& name) const
	{
		return m_Shaders.contains(name);
	}

	void ShaderLibrarySingleton::ReloadAll()
	{
		for (const auto& [filepath, lastModified] : m_LastModifications)
		{
			if (std::filesystem::last_write_time(filepath) > lastModified)
			{
				Load(filepath);
			}
		}
	}
}
