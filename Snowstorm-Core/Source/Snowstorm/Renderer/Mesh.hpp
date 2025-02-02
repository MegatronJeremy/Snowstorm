#pragma once

#include <glm/glm.hpp>

#include "Snowstorm/Core/Base.h"


namespace Snowstorm
{
	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec2 TexCoord;
	};

	class Mesh
	{
	public:
		Mesh(std::vector<Vertex> vertices, std::vector<uint32_t> indices)
			: m_Vertices(std::move(vertices)), m_Indices(std::move(indices))
		{
		}

		[[nodiscard]] const std::vector<Vertex>& GetVertices() const { return m_Vertices; }
		[[nodiscard]] uint32_t GetVertexCount() const { return static_cast<uint32_t>(m_Vertices.size()); }
		[[nodiscard]] const std::vector<uint32_t>& GetIndices() const { return m_Indices; }
		[[nodiscard]] uint32_t GetIndexCount() const { return static_cast<uint32_t>(m_Indices.size()); }

		static Ref<Mesh> CreateCube();
		static Ref<Mesh> CreateQuad();
		static Ref<Mesh> CreateFromFile(const std::string& filepath);

	private:
		std::vector<Vertex> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}
