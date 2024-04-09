#include "pch.h"

#include "VulkanCommandBuffers.h"

namespace Snowstorm
{
	VulkanCommandBuffers::VulkanCommandBuffers(const VkDevice device, const VkCommandPool commandPool,
	                                           const uint32_t count)
		: m_Device(device), m_CommandPool(commandPool), m_Count(count)
	{
		m_CommandBuffers.resize(m_Count);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		// primary: can be submitted to a queue for execution, but cannot be called from other command buffers
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

		const VkResult result = vkAllocateCommandBuffers(m_Device, &allocInfo, m_CommandBuffers.data());
		SS_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocated command buffers!");
	}

	VulkanCommandBuffers::~VulkanCommandBuffers()
	{
		vkFreeCommandBuffers(m_Device, m_CommandPool, m_Count, m_CommandBuffers.data());
	}

	VkCommandBuffer VulkanCommandBuffers::operator[](const uint32_t index) const
	{
		SS_CORE_ASSERT(index < m_Count, "Command buffer index out of range!");
		return m_CommandBuffers[index];
	}
}
