#pragma once

#include "VulkanDevice.h"

namespace Snowstorm
{
	struct VulkanQueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily; // supports drawing commands
		std::optional<uint32_t> presentFamily; // supports presentation commands

		bool IsComplete() const
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}

		static VulkanQueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	};
}
