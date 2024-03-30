#include "pch.h"

#include "VulkanSwapChain.h"

namespace Snowstorm
{
	VulkanSwapChainSupportDetails VulkanSwapChain::QuerySwapChainSupport(
		const VkPhysicalDevice device, const VkSurfaceKHR surface)
	{
		VulkanSwapChainSupportDetails details;

		// device and surface -> core components of the swap chain
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

		// query available surface formats
		uint32_t formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

		if (formatCount != 0)
		{
			details.formats.resize(formatCount); // vector resize to hold all the formats
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
			                                     details.formats.data());
		}

		// query available presentation modes
		uint32_t presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

		if (presentModeCount != 0)
		{
			details.presentModes.resize(presentModeCount);
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
			                                          details.presentModes.data());
		}

		return details;
	}
}
