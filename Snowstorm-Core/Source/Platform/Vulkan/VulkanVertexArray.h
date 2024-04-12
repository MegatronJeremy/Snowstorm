#pragma once
#include "VulkanBuffer.h"
#include "Snowstorm/Renderer/VertexArray.h"

namespace Snowstorm
{
	class VulkanVertexArray final : VertexArray
	{
	public:
		VulkanVertexArray() = default;
		~VulkanVertexArray() override = default;

		VulkanVertexArray(const VulkanVertexArray& other) = delete;
		VulkanVertexArray(VulkanVertexArray&& other) = delete;
		VulkanVertexArray& operator=(const VulkanVertexArray& other) = delete;
		VulkanVertexArray& operator=(VulkanVertexArray&& other) = delete;

		void Bind() const override;

		void Unbind() const override
		{
		}

		void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override
		{
			m_VertexBuffers.push_back(vertexBuffer);
		}

		void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override
		{
			m_IndexBuffer = indexBuffer;
		}

		const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
		const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

	private:
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}
