#pragma once

#include "VulkanDevice.h"
#include "Snowstorm/Renderer/Buffer.h"

namespace Snowstorm
{
	class VulkanVertexBuffer final : public VertexBuffer
	{
	public:
		explicit VulkanVertexBuffer(uint32_t size);
		VulkanVertexBuffer(const float* vertices, uint32_t size);
		~VulkanVertexBuffer() override;

		VulkanVertexBuffer(const VulkanVertexBuffer& other) = delete;
		VulkanVertexBuffer(VulkanVertexBuffer&& other) = delete;
		VulkanVertexBuffer& operator=(const VulkanVertexBuffer& other) = delete;
		VulkanVertexBuffer& operator=(VulkanVertexBuffer&& other) = delete;

		operator VkBuffer() const
		{
			return m_VertexBuffer;
		}

		void Bind() const override;
		void Unbind() const override;

		void SetData(const void* vertices, uint32_t size) override;

		const BufferLayout& GetLayout() const override { return m_Layout; }
		void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

		uint64_t GetHandle() const override
		{
			return reinterpret_cast<uint64_t>(m_VertexBuffer);
		}

	private:
		VkDevice m_Device = VK_NULL_HANDLE;

		VkBuffer m_VertexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory m_VertexBufferMemory = VK_NULL_HANDLE;

		BufferLayout m_Layout;
	};

	class VulkanIndexBuffer final : public IndexBuffer
	{
	public:
		VulkanIndexBuffer(const uint32_t* indices, uint32_t count);
		~VulkanIndexBuffer() override;

		VulkanIndexBuffer(const VulkanIndexBuffer& other) = delete;
		VulkanIndexBuffer(VulkanIndexBuffer&& other) = delete;
		VulkanIndexBuffer& operator=(const VulkanIndexBuffer& other) = delete;
		VulkanIndexBuffer& operator=(VulkanIndexBuffer&& other) = delete;

		operator VkBuffer() const
		{
			return m_IndexBuffer;
		}

		void Bind() const override;
		void Unbind() const override;

		uint32_t GetCount() const override { return m_Count; }

		uint64_t GetHandle() const override
		{
			return reinterpret_cast<uint64_t>(m_IndexBuffer);
		}

	private:
		VkDevice m_Device = VK_NULL_HANDLE;

		VkBuffer m_IndexBuffer = VK_NULL_HANDLE;
		VkDeviceMemory m_IndexBufferMemory = VK_NULL_HANDLE;

		uint32_t m_Count;
	};
}
