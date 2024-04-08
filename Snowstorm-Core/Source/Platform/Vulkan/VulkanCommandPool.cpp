#include "pch.h"

#include "VulkanCommandPool.h"

#include "VulkanQueueFamilyIndices.h"

namespace Snowstorm
{
	VulkanCommandPool::VulkanCommandPool(const VkDevice device, const VkPhysicalDevice physicalDevice,
	                                     const VkSurfaceKHR surface)
		: m_Device(device)
	{
		const VulkanQueueFamilyIndices queueFamilyIndices = VulkanQueueFamilyIndices::FindQueueFamilies(
			physicalDevice, surface);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		// command buffers can be rerecorded individually
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		// we are recording commands for drawing
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		if (vkCreateCommandPool(device, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create command pool!");
		}
	}

	VulkanCommandPool::~VulkanCommandPool()
	{
		vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
	}
}
