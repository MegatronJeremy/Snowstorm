#pragma once

#include <array>

#include "Camera.hpp"

#include "Snowstorm/Renderer/Mesh.hpp"
#include "Snowstorm/Renderer/Material.hpp"
#include "Snowstorm/Renderer/Texture.hpp"
#include "Snowstorm/Renderer/UniformBuffer.hpp"
#include "Snowstorm/Renderer/VertexArray.hpp"
#include "Snowstorm/Core/Base.h"

namespace Snowstorm
{
	struct MeshInstanceData
	{
		glm::mat4 ModelMatrix;
		glm::vec4 Color;
		float TextureIndex;
	};

	struct BatchMeshData
	{
		uint32_t BaseVertex;
		uint32_t BaseIndex;
		uint32_t IndexCount;
	};

	class BatchRenderer3DSingleton final : public Singleton
	{
	public:
		BatchRenderer3DSingleton();

		void BeginBatch(const Camera& camera, const glm::mat4& transform);
		void EndBatch();
		void DrawMesh(const glm::mat4& transform, const Ref<Mesh>& mesh, const Ref<Material>& material);

	private:
		void Flush();

		struct Batch
		{
			Ref<VertexArray> VAO;
			Ref<VertexBuffer> VBO;
			Ref<IndexBuffer> IBO;
			Ref<VertexBuffer> InstanceBuffer;

			std::vector<Vertex> VertexData;
			std::vector<uint32_t> IndexData;
			std::vector<MeshInstanceData> InstanceData;

			std::unordered_map<Ref<Mesh>, BatchMeshData> MeshLookup;

			uint32_t CurrentVertexOffset = 0;
			uint32_t CurrentIndexOffset = 0;
		} m_Batch;

		Ref<UniformBuffer> m_CameraUBO;
		std::array<Ref<Texture>, 32> m_TextureSlots;
		uint32_t m_TextureSlotIndex{1};
		Ref<Material> m_MaterialInstance;
	};
}
