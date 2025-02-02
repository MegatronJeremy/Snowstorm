#pragma once

#include "Shader.hpp"
#include "Texture.hpp"

#include "Snowstorm/Core/Base.h"

namespace Snowstorm
{
	class Material
	{
	public:
		explicit Material(Ref<Shader> shader)
			: m_Shader(std::move(shader))
		{
		}

		void Bind() const;

		template <typename T>
		void SetUniform(const std::string& name, const T& value)
		{
			// TODO add some static assertion here
			m_Uniforms[name] = value;
		}

		// Set textures
		void SetTexture(const std::string& name, Ref<Texture> texture) { m_Textures[name] = std::move(texture); }

		[[nodiscard]] Ref<Shader> GetShader() const { return m_Shader; }

		[[nodiscard]] Ref<Texture> GetTexture(const std::string& name) const;

	private:
		Ref<Shader> m_Shader;
		std::unordered_map<std::string, Shader::UniformValue> m_Uniforms;
		std::unordered_map<std::string, Ref<Texture>> m_Textures;

		// Apply uniforms to the shader
		void ApplyUniform(const std::string& name, const Shader::UniformValue& value) const
		{
			std::visit([&](auto&& v) { m_Shader->SetUniform(name, v); }, value);
		}
	};
}
