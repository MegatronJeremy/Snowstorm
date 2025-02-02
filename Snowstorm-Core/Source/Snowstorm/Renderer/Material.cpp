#include "Material.hpp"

namespace Snowstorm
{
	void Material::Bind() const
	{
		m_Shader->Bind();

		// Bind all stored uniforms
		for (const auto& [name, value] : m_Uniforms)
		{
			ApplyUniform(name, value);
		}

		// Bind all stored textures
		uint32_t slot = 0;
		for (const auto& [name, texture] : m_Textures)
		{
			texture->Bind(slot);
			m_Shader->SetUniform(name, static_cast<int>(slot));
			slot++;
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
}
