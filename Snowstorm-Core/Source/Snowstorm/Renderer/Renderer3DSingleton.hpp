#pragma once

#include "Camera.hpp"
#include "Material.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"
#include "UniformBuffer.hpp"
#include "VertexArray.hpp"

namespace Snowstorm
{
	struct MeshInstanceData
	{
		glm::mat4 ModelMatrix;
		float TextureIndex;
	};

	struct BatchData
	{
		Ref<Mesh> Mesh;
		Ref<Material> Material;
		Ref<VertexArray> VAO;
		Ref<VertexBuffer> VBO;
		Ref<VertexBuffer> InstanceBuffer;
		Ref<IndexBuffer> IBO;
		std::vector<MeshInstanceData> Instances;
	};

	class Renderer3DSingleton final : public Singleton
	{
	public:
		Renderer3DSingleton();
		void BeginScene(const Camera& camera, const glm::mat4& transform);
		void EndScene();
		void DrawMesh(const glm::mat4& transform, const Ref<Mesh>& mesh, const Ref<Material>& material);
		void Flush();

	private:
		void FlushBatch(BatchData& batch) const;
		float GetTextureIndex(const Ref<Texture>& texture);

		Ref<UniformBuffer> m_CameraUBO;
		std::vector<BatchData> m_Batches;

		static constexpr uint32_t MAX_TEXTURE_SLOTS = 32;
		Ref<Texture> m_TextureSlots[MAX_TEXTURE_SLOTS];
		std::unordered_map<Ref<Texture>, uint32_t> m_TextureSlotMap;
		uint32_t m_TextureSlotIndex = 1;
	};
}
