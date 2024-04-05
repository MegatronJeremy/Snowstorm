#include "pch.h"
#include "Shader.h"

#include "Renderer.h"
#include "Platform/Vulkan/VulkanShader.h"

namespace Snowstorm
{
#pragma region Shader
	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanShader>(filepath);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name,
	                           const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case RendererAPI::API::None:
			SS_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		case RendererAPI::API::Vulkan:
			return CreateRef<VulkanShader>(name, vertexSrc, fragmentSrc);
		}

		SS_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
#pragma endregion

#pragma region ShaderLibrary
	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		SS_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		SS_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.find(name) != m_Shaders.end();
	}
#pragma endregion
}
