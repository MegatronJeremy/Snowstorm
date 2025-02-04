#include "Renderer3DSingleton.hpp"

#include "RenderCommand.hpp"

namespace Snowstorm
{
	Renderer3DSingleton::Renderer3DSingleton()
	{
		m_CameraUBO = UniformBuffer::Create(sizeof(glm::mat4), 0); // Binding = 0

		// Default white texture
		m_TextureSlots[0] = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		m_TextureSlots[0]->SetData(&whiteTextureData, sizeof(uint32_t));
	}

	void Renderer3DSingleton::BeginScene(const Camera& camera, const glm::mat4& transform)
	{
		const glm::mat4 viewProj = camera.GetProjection() * inverse(transform);
		m_CameraUBO->SetData(&viewProj, sizeof(glm::mat4));

		m_Batches.clear();
		m_TextureSlotIndex = 1;
		m_TextureSlotMap.clear();
	}

	void Renderer3DSingleton::EndScene()
	{
		Flush();
	}

	void Renderer3DSingleton::DrawMesh(const glm::mat4& transform, const Ref<Mesh>& mesh, const Ref<Material>& material)
	{
		BatchData* batch = nullptr;
		for (auto& b : m_Batches)
		{
			if (b.Mesh == mesh && b.Material == material)
			{
				batch = &b;
				break;
			}
		}

		if (!batch)
		{
			// Create new batch
			BatchData newBatch;
			newBatch.Mesh = mesh;
			newBatch.Material = material;
			newBatch.VAO = VertexArray::Create();
			newBatch.VAO->Bind();

			newBatch.VBO = VertexBuffer::Create(mesh->GetVertices().data(), mesh->GetVertexCount() * sizeof(Vertex));
			newBatch.IBO = IndexBuffer::Create(mesh->GetIndices().data(), mesh->GetIndexCount());

			// **Get vertex attributes dynamically from the material**
			const auto vertexLayout = material->GetVertexLayout();
			const auto instanceLayout = material->GetInstanceLayout();

			newBatch.VBO->SetLayout(vertexLayout);

			newBatch.InstanceBuffer = VertexBuffer::Create(1000 * sizeof(MeshInstanceData));
			newBatch.InstanceBuffer->SetLayout(instanceLayout);

			newBatch.VAO->AddVertexBuffer(newBatch.VBO);
			newBatch.VAO->AddVertexBuffer(newBatch.InstanceBuffer);
			newBatch.VAO->SetIndexBuffer(newBatch.IBO);

			m_Batches.push_back(std::move(newBatch));
			batch = &m_Batches.back();
		}

		// Add instance data
		MeshInstanceData instance;
		instance.ModelMatrix = transform;

		// **Get the Albedo texture from material**
		const Ref<Texture> albedoTexture = material->GetTexture("AlbedoTexture");
		instance.TextureIndex = GetTextureIndex(albedoTexture);

		batch->Instances.push_back(instance);

		if (batch->Instances.size() >= 1000)
		{
			FlushBatch(*batch);
			batch->Instances.clear();
		}
	}

	void Renderer3DSingleton::Flush()
	{
		for (auto& batch : m_Batches)
		{
			FlushBatch(batch);
		}
	}

	void Renderer3DSingleton::FlushBatch(BatchData& batch) const
	{
		if (batch.Instances.empty()) return;

		batch.InstanceBuffer->SetData(batch.Instances.data(), batch.Instances.size() * sizeof(MeshInstanceData));

		batch.VAO->Bind();

		for (uint32_t i = 0; i < m_TextureSlotIndex; i++)
		{
			m_TextureSlots[i]->Bind(i);
		}

		batch.Material->Bind();

		RenderCommand::DrawIndexedInstanced(batch.VAO, batch.Mesh->GetIndexCount(),
		                                    static_cast<uint32_t>(batch.Instances.size()));

		batch.Instances.clear();
	}

	float Renderer3DSingleton::GetTextureIndex(const Ref<Texture>& texture)
	{
		if (!texture)
			return 0.0f;

		if (const auto it = m_TextureSlotMap.find(texture); it != m_TextureSlotMap.end())
			return static_cast<float>(it->second);

		if (m_TextureSlotIndex >= MAX_TEXTURE_SLOTS)
		{
			Flush();
			m_TextureSlotIndex = 1;
			m_TextureSlotMap.clear();
		}

		m_TextureSlots[m_TextureSlotIndex] = texture;
		m_TextureSlotMap[texture] = m_TextureSlotIndex;
		return static_cast<float>(m_TextureSlotIndex++);
	}
}
