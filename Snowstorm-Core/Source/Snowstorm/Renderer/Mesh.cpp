#include "Mesh.hpp"

#include "RenderCommand.hpp"
#include "Snowstorm/Core/Log.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Snowstorm
{
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
		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(filepath,
		                                         aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_JoinIdenticalVertices);

		if (!scene || !scene->mRootNode || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE)
		{
			SS_CORE_ERROR("Failed to load mesh: {} | Assimp Error: {}", filepath, importer.GetErrorString());
			return nullptr;
		}

		std::vector<Vertex> vertices;
		std::vector<uint32_t> indices;

		for (uint32_t i = 0; i < scene->mNumMeshes; i++)
		{
			const aiMesh* mesh = scene->mMeshes[i];

			const uint32_t baseIndex = static_cast<uint32_t>(vertices.size());

			for (uint32_t j = 0; j < mesh->mNumVertices; j++)
			{
				Vertex vertex;
				vertex.Position = {mesh->mVertices[j].x, mesh->mVertices[j].y, mesh->mVertices[j].z};

				if (mesh->HasNormals())
				{
					vertex.Normal = {mesh->mNormals[j].x, mesh->mNormals[j].y, mesh->mNormals[j].z};
				}
				else
				{
					vertex.Normal = {0.0f, 1.0f, 0.0f}; // Default normal if missing
				}

				if (mesh->HasTextureCoords(0))
				{
					vertex.TexCoord = {mesh->mTextureCoords[0][j].x, 1.0f - mesh->mTextureCoords[0][j].y};
				}
				else
				{
					// Simple planar mapping (XZ plane)
					vertex.TexCoord = {vertex.Position.x, vertex.Position.z};
				}

				vertices.push_back(vertex);
			}

			for (uint32_t j = 0; j < mesh->mNumFaces; j++)
			{
				const aiFace& face = mesh->mFaces[j];
				for (uint32_t k = 0; k < face.mNumIndices; k++)
				{
					indices.push_back(baseIndex + face.mIndices[k]);
				}
			}
		}

		return CreateRef<Mesh>(vertices, indices);
	}
}
