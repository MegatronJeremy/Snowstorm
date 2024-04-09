#pragma once
#include "VulkanCommandPool.h"

namespace Snowstorm
{
	class VulkanCommandBuffers
	{
	public:
		VulkanCommandBuffers(VkDevice device, VkCommandPool commandPool, uint32_t count);
		~VulkanCommandBuffers();

		VulkanCommandBuffers(const VulkanCommandBuffers& other) = delete;
		VulkanCommandBuffers(VulkanCommandBuffers&& other) = delete;
		VulkanCommandBuffers& operator=(const VulkanCommandBuffers& other) = delete;
		VulkanCommandBuffers& operator=(VulkanCommandBuffers&& other) = delete;

		VkCommandBuffer operator[](uint32_t index) const;

	private:
		VkDevice m_Device;
		VkCommandPool m_CommandPool;

		std::vector<VkCommandBuffer> m_CommandBuffers;
		uint32_t m_Count;
	};
}
