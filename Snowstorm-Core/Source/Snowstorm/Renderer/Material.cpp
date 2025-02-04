#include "Material.hpp"

#include <ranges>

namespace Snowstorm
{
	Material::Material(Ref<Shader> shader): m_Shader(std::move(shader))
	{
		m_Uniforms["u_Color"] = glm::vec4(1.0f); // Default color

		std::vector<int32_t> samplers(32); // TODO don't hardcode this (MAX TEXTURE SAMPLES)
		std::iota(samplers.begin(), samplers.end(), 0);
		m_Uniforms["u_Textures"] = samplers;
	}

	void Material::Bind() const
	{
		m_Shader->Bind();

		// Bind all stored uniforms
		for (const auto& [name, value] : m_Uniforms)
		{
			ApplyUniform(name, value);
		}
	}

	Ref<Texture> Material::GetTexture(const std::string& name) const
	{
		if (const auto it = m_Textures.find(name); it != m_Textures.end())
		{
			return it->second;
		}
		return nullptr; // Return null if texture not found
	}

	std::vector<BufferElement> Material::GetVertexLayout() const
	{
		// Base attributes (Every mesh should have these)
		std::vector<BufferElement> layout = {
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float3, "a_Normal"},
			{ShaderDataType::Float2, "a_TexCoord"}
		};

		// Add extra attributes dynamically
		layout.insert(layout.end(), m_ExtraVertexAttributes.begin(), m_ExtraVertexAttributes.end());

		return layout;
	}

	std::vector<BufferElement> Material::GetInstanceLayout() const
	{
		return {
			{ShaderDataType::Mat4, "a_ModelMatrix", true},
			{ShaderDataType::Float, "a_TextureIndex", true}
		};
	}

	void Material::ApplyUniform(const std::string& name, const Shader::UniformValue& value) const
	{
		std::visit([&](auto&& v) { m_Shader->SetUniform(name, v); }, value);
	}
}
