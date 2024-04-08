#pragma once
#include "VulkanInstance.h"

namespace Snowstorm
{
	class VulkanDevice
	{
	public:
		explicit VulkanDevice(VkSurfaceKHR surface);

		VulkanDevice(const VulkanDevice& other) = delete;
		VulkanDevice(VulkanDevice&& other) = delete;
		VulkanDevice& operator=(const VulkanDevice& other) = delete;
		VulkanDevice& operator=(VulkanDevice&& other) = delete;

		~VulkanDevice();

		VkPhysicalDevice GetVkPhysicalDevice() const
		{
			return m_VkPhysicalDevice;
		}

		VkDevice GetVkDevice() const
		{
			return m_VkDevice;
		}

		VkQueue GetVkGraphicsQueue() const
		{
			return m_GraphicsQueue;
		}

		VkQueue GetVkPresentQueue() const
		{
			return m_PresentQueue;
		}

	private:
		void PickPhysicalDevice();

		void CreateLogicalDevice();

		bool CheckDeviceExtensionSupport(VkPhysicalDevice device) const;

		int RateDeviceSuitability(VkPhysicalDevice device) const;

	private:
		VkPhysicalDevice m_VkPhysicalDevice = VK_NULL_HANDLE;
		VkDevice m_VkDevice = VK_NULL_HANDLE; // logical m_VkDevice - to interface with the physical m_VkDevice

		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;

		VkQueue m_GraphicsQueue = VK_NULL_HANDLE; // implicitly destroyed when the device is destroyed
		VkQueue m_PresentQueue = VK_NULL_HANDLE;

		std::vector<const char*> m_DeviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
	};
}
