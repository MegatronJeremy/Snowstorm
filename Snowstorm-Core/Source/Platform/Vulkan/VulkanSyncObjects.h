#pragma once
#include "VulkanDevice.h"

namespace Snowstorm
{
	class VulkanSemaphore
	{
	public:
		explicit VulkanSemaphore(VkDevice device);
		~VulkanSemaphore();

		VulkanSemaphore(const VulkanSemaphore& other) = delete;
		VulkanSemaphore(VulkanSemaphore&& other) = delete;
		VulkanSemaphore& operator=(const VulkanSemaphore& other) = delete;
		VulkanSemaphore& operator=(VulkanSemaphore&& other) = delete;

		void Wait();
		void Signal();

	private:
		VkDevice m_Device;

		VkSemaphore m_Semaphore;
	};

	class VulkanFence
	{
	public:
		explicit VulkanFence(VkDevice device, bool signaled = false);
		~VulkanFence();

		VulkanFence(const VulkanFence& other) = delete;
		VulkanFence(VulkanFence&& other) = delete;
		VulkanFence& operator=(const VulkanFence& other) = delete;
		VulkanFence& operator=(VulkanFence&& other) = delete;

		void Wait() const;
		void Reset() const;

	private:
		VkDevice m_Device;

		VkFence m_Fence;
	};
}
