#include "Mesh.hpp"

#include "RenderCommand.hpp"

#include "Snowstorm/Core/Log.h"

#include <tiny_obj_loader.h>

namespace Snowstorm
{
	Ref<Mesh> Mesh::CreateCube()
	{
		std::vector<Vertex> vertices = {
			// Front face
			{{-0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}},
			{{0.5f, -0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}},
			{{0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}},
			{{-0.5f, 0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},

			// Back face
			{{-0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 0.0f}},
			{{0.5f, -0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 0.0f}},
			{{0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {0.0f, 1.0f}},
			{{-0.5f, 0.5f, -0.5f}, {0.0f, 0.0f, -1.0f}, {1.0f, 1.0f}},
		};

		std::vector<uint32_t> indices = {
			// Front
			0, 1, 2, 2, 3, 0,
			// Back
			4, 5, 6, 6, 7, 4,
			// Left
			4, 0, 3, 3, 7, 4,
			// Right
			1, 5, 6, 6, 2, 1,
			// Top
			3, 2, 6, 6, 7, 3,
			// Bottom
			4, 5, 1, 1, 0, 4
		};

		return CreateRef<Mesh>(vertices, indices);
	}

	Ref<Mesh> Mesh::CreateQuad()
	{
		std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 0.0f}}, // Bottom-left
			{{0.5f, -0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 0.0f}}, // Bottom-right
			{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {1.0f, 1.0f}}, // Top-right
			{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}}, // Top-left
		};

		std::vector<uint32_t> indices = {
			0, 1, 2, 2, 3, 0
		};

		return CreateRef<Mesh>(vertices, indices);
	}

	Ref<Mesh> Mesh::CreateFromFile(const std::string& filepath)
	{
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str()))
		{
			SS_CORE_ERROR("Failed to load mesh: {}", filepath);
			return nullptr;
		}

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		for (const auto& shape : shapes)
		{
			for (const auto& index : shape.mesh.indices)
			{
				Vertex vertex;
				vertex.Position = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				if (!attrib.normals.empty())
				{
					vertex.Normal = {
						attrib.normals[3 * index.normal_index + 0],
						attrib.normals[3 * index.normal_index + 1],
						attrib.normals[3 * index.normal_index + 2]
					};
				}

				if (!attrib.texcoords.empty())
				{
					vertex.TexCoord = {
						attrib.texcoords[2 * index.texcoord_index + 0],
						1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
					};
				}

				vertices.push_back(vertex);
				indices.push_back(static_cast<uint32_t>(indices.size()));
			}
		}

		return CreateRef<Mesh>(vertices, indices);
	}
}
