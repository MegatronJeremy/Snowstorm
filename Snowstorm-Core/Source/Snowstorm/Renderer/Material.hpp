#pragma once

#pragma once

#include <numeric>

#include "Buffer.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Snowstorm/Core/Base.h"

namespace Snowstorm
{
	class Material
	{
	public:
		explicit Material(Ref<Shader> shader);

		void Bind() const;

		template <typename T>
		void SetUniform(const std::string& name, const T& value)
		{
			m_Uniforms[name] = value;
		}

		void SetTexture(const std::string& name, Ref<Texture> texture) { m_Textures[name] = std::move(texture); }

		void SetColor(const glm::vec4& color) { m_Uniforms["u_Color"] = color; }

		[[nodiscard]] Ref<Shader> GetShader() const { return m_Shader; }

		[[nodiscard]] Ref<Texture> GetTexture(const std::string& name) const;

		[[nodiscard]] glm::vec4& GetColor() { return std::get<glm::vec4>(m_Uniforms["u_Color"]); }

		// Vertex & Instance Layout Functions
		[[nodiscard]] std::vector<BufferElement> GetVertexLayout() const;
		[[nodiscard]] std::vector<BufferElement> GetInstanceLayout() const;

		void AddVertexAttribute(const BufferElement& attribute) { m_ExtraVertexAttributes.push_back(attribute); }

	private:
		Ref<Shader> m_Shader;
		std::unordered_map<std::string, Shader::UniformValue> m_Uniforms;
		std::unordered_map<std::string, Ref<Texture>> m_Textures;
		std::vector<BufferElement> m_ExtraVertexAttributes;

		void ApplyUniform(const std::string& name, const Shader::UniformValue& value) const;
	};
}
