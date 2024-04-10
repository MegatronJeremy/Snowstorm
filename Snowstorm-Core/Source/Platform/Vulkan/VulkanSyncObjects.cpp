#include "pch.h"
#include "VulkanSyncObjects.h"

namespace Snowstorm
{
	VulkanSemaphore::VulkanSemaphore(const VkDevice device)
		: m_Device(device)
	{
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO; // current version


		const VkResult result = vkCreateSemaphore(device, &semaphoreInfo, nullptr, &m_Semaphore);
		SS_CORE_ASSERT(result == VK_SUCCESS, "Failed to create semaphore!");
	}

	VulkanSemaphore::~VulkanSemaphore()
	{
		vkDestroySemaphore(m_Device, m_Semaphore, nullptr);
	}


	VulkanSemaphore::operator VkSemaphore_T*() const
	{
		return m_Semaphore;
	}

	VulkanFence::VulkanFence(const VkDevice device, const bool signaled)
		: m_Device(device)
	{
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		if (signaled)
			fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // create signaled for the first frame

		const VkResult result = vkCreateFence(device, &fenceInfo, nullptr, &m_Fence);
		SS_CORE_ASSERT(result == VK_SUCCESS, "Failed to create fence!");
	}

	VulkanFence::~VulkanFence()
	{
		vkDestroyFence(m_Device, m_Fence, nullptr);
	}

	VulkanFence::operator VkFence() const
	{
		return m_Fence;
	}

	void VulkanFence::Wait() const
	{
		vkWaitForFences(m_Device, 1, &m_Fence, VK_TRUE, UINT64_MAX);
	}

	void VulkanFence::Reset() const
	{
		vkResetFences(m_Device, 1, &m_Fence);
	}
}
