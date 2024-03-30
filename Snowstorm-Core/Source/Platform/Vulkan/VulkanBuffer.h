#pragma once

#include "Snowstorm/Renderer/Buffer.h"

namespace Snowstorm
{
	class VulkanVertexBuffer final : public VertexBuffer
	{
	public:
		explicit VulkanVertexBuffer(uint32_t size);
		VulkanVertexBuffer(const float* vertices, uint32_t size);

		VulkanVertexBuffer(const VulkanVertexBuffer& other) = delete;
		VulkanVertexBuffer(VulkanVertexBuffer&& other) = delete;
		VulkanVertexBuffer& operator=(const VulkanVertexBuffer& other) = delete;
		VulkanVertexBuffer& operator=(VulkanVertexBuffer&& other) = delete;

		~VulkanVertexBuffer() override;

		void Bind() const override;
		void Unbind() const override;

		void SetData(const void* data, uint32_t size) override;

		const BufferLayout& GetLayout() const override { return m_Layout; }
		void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};

	class VulkanIndexBuffer final : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(uint32_t* indices, uint32_t count);

		VulkanIndexBuffer(const VulkanIndexBuffer& other) = delete;
		VulkanIndexBuffer(VulkanIndexBuffer&& other) = delete;
		VulkanIndexBuffer& operator=(const VulkanIndexBuffer& other) = delete;
		VulkanIndexBuffer& operator=(VulkanIndexBuffer&& other) = delete;

		~VulkanIndexBuffer() override;

		void Bind() const override;
		void Unbind() const override;

		uint32_t GetCount() const override { return m_Count; }

	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};
}
