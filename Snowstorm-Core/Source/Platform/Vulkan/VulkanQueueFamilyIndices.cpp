#include "pch.h"

#include "VulkanQueueFamilyIndices.h"

namespace Snowstorm
{
	VulkanQueueFamilyIndices VulkanQueueFamilyIndices::FindQueueFamilies(
		const VkPhysicalDevice device, const VkSurfaceKHR surface)
	{
		VulkanQueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies)
		{
			// we need to find at least one queue family that support graphics commands and presentation commands
			if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphicsFamily = i;
			}

			VkBool32 presentSupport = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

			if (presentSupport)
			{
				indices.presentFamily = i;
			}

			// if found both break immediately
			if (indices.IsComplete())
			{
				break;
			}

			i++;
		}

		return indices;
	}
}
