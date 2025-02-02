#pragma once

#include <vulkan/vulkan_core.h>

#include "Snowstorm/Renderer/VertexArray.hpp"

namespace Snowstorm
{
	class VulkanVertexArray final : public VertexArray
	{
	public:
		VulkanVertexArray() = default;
		~VulkanVertexArray() override = default;

		VulkanVertexArray(const VulkanVertexArray& other) = delete;
		VulkanVertexArray(VulkanVertexArray&& other) = delete;
		VulkanVertexArray& operator=(const VulkanVertexArray& other) = delete;
		VulkanVertexArray& operator=(VulkanVertexArray&& other) = delete;

		void Bind() const override
		{
		}

		void Unbind() const override
		{
		}

		void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;

		void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		void SetInstanceDivisor(uint32_t index, uint32_t divisor) const override;

		[[nodiscard]] const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
		[[nodiscard]] const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

		static VkVertexInputBindingDescription GetBindingDescription();

		static std::vector<VkVertexInputAttributeDescription> GetAttributeDescriptions();

	private:
		static inline VkVertexInputBindingDescription s_BindingDescription;
		static inline std::vector<VkVertexInputAttributeDescription> s_AttributeDescriptions;

		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
	};
}
