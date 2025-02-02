#include "BatchRenderer3DSingleton.hpp"

#include <array>

#include "RenderCommand.hpp"


namespace Snowstorm
{
	BatchRenderer3DSingleton::BatchRenderer3DSingleton()
	{
		m_CameraUBO = UniformBuffer::Create(sizeof(glm::mat4), 0); // Binding = 0

		m_Batch.VAO = VertexArray::Create();
		m_Batch.VAO->Bind();

		m_Batch.VBO = VertexBuffer::Create(50000 * sizeof(Vertex));
		m_Batch.IBO = IndexBuffer::Create(nullptr, 150000);
		m_Batch.InstanceBuffer = VertexBuffer::Create(5000 * sizeof(MeshInstanceData));

		m_Batch.VBO->SetLayout({
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float3, "a_Normal"},
			{ShaderDataType::Float2, "a_TexCoord"}
		});

		m_Batch.InstanceBuffer->SetLayout({
			{ShaderDataType::Mat4, "a_ModelMatrix", true},
			{ShaderDataType::Float4, "a_Color", true},
			{ShaderDataType::Float, "a_TextureIndex", true}
		});

		m_Batch.VAO->AddVertexBuffer(m_Batch.VBO);
		m_Batch.VAO->AddVertexBuffer(m_Batch.InstanceBuffer);
		m_Batch.VAO->SetIndexBuffer(m_Batch.IBO);

		m_TextureSlots[0] = Texture2D::Create(1, 1);
		uint32_t whiteTextureData = 0xffffffff;
		m_TextureSlots[0]->SetData(&whiteTextureData, sizeof(uint32_t));
	}

	void BatchRenderer3DSingleton::BeginBatch(const Camera& camera, const glm::mat4& transform)
	{
		const glm::mat4 viewProj = camera.GetProjection() * inverse(transform);
		m_CameraUBO->SetData(&viewProj, sizeof(glm::mat4));

		m_Batch.InstanceData.clear();
		m_TextureSlotIndex = 1;
	}

	void BatchRenderer3DSingleton::EndBatch()
	{
		Flush();
	}

	void BatchRenderer3DSingleton::DrawMesh(const glm::mat4& transform, const Ref<Mesh>& mesh, const Ref<Material>& material)
	{
		if (m_Batch.InstanceData.size() >= 5000)
		{
			Flush();
		}

		const auto it = m_Batch.MeshLookup.find(mesh);
		if (it == m_Batch.MeshLookup.end())
		{
			BatchMeshData meshData;
			meshData.BaseVertex = m_Batch.CurrentVertexOffset;
			meshData.BaseIndex = m_Batch.CurrentIndexOffset;
			meshData.IndexCount = mesh->GetIndexCount();

			m_Batch.VertexData.insert(m_Batch.VertexData.end(), mesh->GetVertices().begin(), mesh->GetVertices().end());

			std::vector<uint32_t> adjustedIndices = mesh->GetIndices();
			for (uint32_t& index : adjustedIndices)
			{
				index += meshData.BaseVertex;
			}
			m_Batch.IndexData.insert(m_Batch.IndexData.end(), adjustedIndices.begin(), adjustedIndices.end());

			m_Batch.CurrentVertexOffset += mesh->GetVertexCount();
			m_Batch.CurrentIndexOffset += mesh->GetIndexCount();
			m_Batch.MeshLookup[mesh] = meshData;
		}

		MeshInstanceData instance;
		instance.ModelMatrix = transform;
		instance.Color = glm::vec4{1.0f, 0.0f, 0.0f, 1.0f};
		instance.TextureIndex = 0.0f;

		// TODO have to do something else with this
		m_MaterialInstance = material;

		if (const Ref<Texture> albedoTexture = material->GetTexture("u_AlbedoTexture"))
		{
			for (uint32_t i = 1; i < m_TextureSlotIndex; i++)
			{
				if (*m_TextureSlots[i] == *albedoTexture)
				{
					instance.TextureIndex = static_cast<float>(i);
					break;
				}
			}

			if (instance.TextureIndex == 0.0f)
			{
				instance.TextureIndex = static_cast<float>(m_TextureSlotIndex);
				m_TextureSlots[m_TextureSlotIndex] = albedoTexture;
				m_TextureSlotIndex++;
			}
		}

		m_Batch.InstanceData.push_back(instance);
	}

	void BatchRenderer3DSingleton::Flush()
	{
		if (m_Batch.InstanceData.empty()) return;

		m_Batch.VBO->SetData(m_Batch.VertexData.data(), m_Batch.VertexData.size() * sizeof(Vertex));
		m_Batch.VertexData.clear();

		m_Batch.IBO->SetData(m_Batch.IndexData.data(), m_Batch.IndexData.size() * sizeof(uint32_t));
		m_Batch.IndexData.clear();

		m_Batch.InstanceBuffer->SetData(m_Batch.InstanceData.data(), m_Batch.InstanceData.size() * sizeof(MeshInstanceData));
		m_Batch.VAO->Bind();

		for (uint32_t i = 0; i < m_TextureSlotIndex; i++)
		{
			m_TextureSlots[i]->Bind(i);
		}

		m_MaterialInstance->GetShader()->Bind();

		RenderCommand::DrawIndexedInstanced(m_Batch.VAO, m_Batch.CurrentIndexOffset, static_cast<uint32_t>(m_Batch.InstanceData.size()));

		m_Batch.InstanceData.clear();
		m_Batch.MeshLookup.clear();
		m_Batch.CurrentVertexOffset = 0;
		m_Batch.CurrentIndexOffset = 0;
		m_TextureSlotIndex = 1;
	}
}
