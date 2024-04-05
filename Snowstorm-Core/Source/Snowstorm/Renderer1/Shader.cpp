#include "pch.h"
#include "Shader.h"

#include <utility>

#include "Snowstorm/Renderer/Renderer.h"
#include "Snowstorm/Platform/Vulkan/VulkanShader.h"

#if SS_HAS_SHADER_COMPILER
#include "Snowstorm/Platform/Vulkan/ShaderCompiler/VulkanShaderCompiler.h"
#endif

#include "Snowstorm/Renderer/RendererAPI.h"
#include "Snowstorm/Renderer/ShaderPack.h"

namespace Snowstorm {

	Ref<Shader> Shader::Create(const std::string& filepath, bool forceCompile, bool disableOptimization)
	{
		Ref<Shader> result = nullptr;

		switch (RendererAPI::Current())
		{
			case RendererAPIType::None: return nullptr;
			case RendererAPIType::Vulkan:
				result = Ref<VulkanShader>::Create(filepath, forceCompile, disableOptimization);
				break;
		}
		return result;
	}

	Ref<Shader> Shader::CreateFromString(const std::string& source)
	{
		Ref<Shader> result = nullptr;

		switch (RendererAPI::Current())
		{
			case RendererAPIType::None: return nullptr;
		}
		return result;
	}

	ShaderLibrary::ShaderLibrary()
	{
	}

	ShaderLibrary::~ShaderLibrary()
	{
	}

	void ShaderLibrary::Add(const Snowstorm::Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		SS_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Load(std::string_view path, bool forceCompile, bool disableOptimization)
	{
		Ref<Shader> shader;
		if (!forceCompile && m_ShaderPack)
		{
			if (m_ShaderPack->Contains(path))
				shader = m_ShaderPack->LoadShader(path);
		}
		else
		{
			// Try compile from source
			// Unavailable at runtime
#if SS_HAS_SHADER_COMPILER
			shader = VulkanShaderCompiler::Compile(path, forceCompile, disableOptimization);
#endif
		}

		auto& name = shader->GetName();
		SS_CORE_ASSERT(m_Shaders.find(name) == m_Shaders.end());
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Load(std::string_view name, const std::string& path)
	{
		SS_CORE_ASSERT(m_Shaders.find(std::string(name)) == m_Shaders.end());
		m_Shaders[std::string(name)] = Shader::Create(path);
	}

	void ShaderLibrary::LoadShaderPack(const std::filesystem::path& path)
	{
		m_ShaderPack = Ref<ShaderPack>::Create(path);
		if (!m_ShaderPack->IsLoaded())
		{
			m_ShaderPack = nullptr;
			SS_CORE_ERROR("Could not load shader pack: {}", path.string());
		}
	}

	const Ref<Shader>& ShaderLibrary::Get(const std::string& name) const
	{
		SS_CORE_ASSERT(m_Shaders.find(name) != m_Shaders.end());
		return m_Shaders.at(name);
	}

	ShaderUniform::ShaderUniform(std::string name, const ShaderUniformType type, const uint32_t size, const uint32_t offset)
		: m_Name(std::move(name)), m_Type(type), m_Size(size), m_Offset(offset)
	{
	}

	constexpr std::string_view ShaderUniform::UniformTypeToString(const ShaderUniformType type)
	{
		if (type == ShaderUniformType::Bool)
		{
			return std::string("Boolean");
		}
		else if (type == ShaderUniformType::Int)
		{
			return std::string("Int");
		}
		else if (type == ShaderUniformType::Float)
		{
			return std::string("Float");
		}

		return std::string("None");
	}

}
