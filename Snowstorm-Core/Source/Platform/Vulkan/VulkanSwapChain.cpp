#include "pch.h"

#include "VulkanSwapChain.h"

#include <GLFW/glfw3.h>

#include "VulkanQueueFamilyIndices.h"

namespace Snowstorm
{
	VulkanSwapChain::VulkanSwapChain(const VkPhysicalDevice physicalDevice, const VkDevice device,
	                                 const VkSurfaceKHR surface,
	                                 GLFWwindow* window)
		: m_Device(device)
	{
		const VulkanSwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice, surface);

		const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		const VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
		const VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities, window);

		// recommended to request at least one more image than the minimum -> else we have to wait for internal operations
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		// also do not exceed the maximum number of images while doing this (0 is a special value - there is no max)
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		// Now create the actual swap chain
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

		// first specify the surface the swap chain should be tied to
		createInfo.surface = surface;

		// details of the swap chain images go next
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;

		// amount of layers each image consists of -> always 1 unless with a stereoscopic 3D app
		createInfo.imageArrayLayers = 1;

		// what kind of operations we'll use the images for -> we render directly to them here
		// also possible: rendering images to a separate image first, then do post-processing (VK_IMAGE_USAGE_TRANSFER_DST_BIT), then transfer to a swap chain image
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		// Next specify how to handle swap chain images used across multiple queue families
		// This is the case when the graphics queue family is different from the presentation queue
		const VulkanQueueFamilyIndices indices = VulkanQueueFamilyIndices::FindQueueFamilies(physicalDevice, surface);

		const uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		// draw from the graphics queue -> then submit to the presentation queue
		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			// images can be used concurrently across multiple queue families
			// TODO we will do ownership transfer later

			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			// best performance - explicit ownership transfer

			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		// if we want to do a pre-transform, like rotating by 90 degrees, etc.
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

		// specifies if the alpha channel should be used for blending with other windows in the window system
		// we almost always want to ignore this
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		// if clipped is VK_TRUE -> we don't care about the color of pixels that are obscured (if another window is in front for example)
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		// old swap chain -> with Vulkan it is possible that another swap chain becomes invalid/unoptimized (if the window is resized)
		// in this case, the swap chain needs to be recreated from scratch!
		// TODO we will do this in the future, for now, only assume one swap chain
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create swap chain!");
		}

		// we only specified a minimum number of images -> the implementation is allowed to create a swap chain with more
		vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, nullptr);
		m_SwapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, m_SwapChain, &imageCount, m_SwapChainImages.data());

		// store these for later
		m_SwapChainImageFormat = surfaceFormat.format;
		m_SwapChainExtent = extent;
	}

	VulkanSwapChain::~VulkanSwapChain()
	{
		for (const auto& swapChainFramebuffer : swapChainFramebuffers)
		{
			vkDestroyFramebuffer(m_Device, swapChainFramebuffer, nullptr);
		}

		for (const auto& swapChainImageView : m_SwapChainImageViews)
		{
			vkDestroyImageView(m_Device, swapChainImageView, nullptr);
		}

		vkDestroySwapchainKHR(m_Device, m_SwapChain, nullptr);
		
	}

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


	VkSurfaceFormatKHR VulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
	{
		for (const auto& availableFormat : availableFormats)
		{
			// this is optimal -> we always want sRGB
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
				availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return availableFormat;
			}
		}

		// if it fails, ok to settle with the first format available
		return availableFormats[0];
	}

	VkPresentModeKHR VulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
	{
		// VK_PRESENT_MODE_FIFO_KHR -> always guaranteed -> this is vertical sync (use on mobile phones)
		// VK_PRESENT_MODE_MAILBOX_KHR -> triple buffering -> good if energy expenditure is not an issue

		for (const auto& availablePresentMode : availablePresentModes)
		{
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window)
	{
		// swap extent - resolution of the swap chain -> (almost always) equal to the
		// resolution of the window we are drawing in -> in pixels
		// width and height -> in currentExtent
		// we can differ here sometimes -> set the currentExtent to the max value of uint32_t
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
		{
			return capabilities.currentExtent; // we want to match it here
		}

		// certain windows managers allow differing values
		int width, height;
		glfwGetFramebufferSize(window, &width, &height); // query the resolution of the window in pixel

		VkExtent2D actualExtent = {
			static_cast<uint32_t>(width),
			static_cast<uint32_t>(height)
		};

		// match against the minimum and maximum image extent
		actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
		                                capabilities.maxImageExtent.width);
		actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
		                                 capabilities.maxImageExtent.height);

		return actualExtent;
	}

	void VulkanSwapChain::CreateImageViews()
	{
		m_SwapChainImageViews.resize(m_SwapChainImages.size());

		for (size_t i = 0; i < m_SwapChainImages.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = m_SwapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = m_SwapChainImageFormat;

			// stick to the default mappings -> we can do stuff like monochrome here instead
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			// subresource range -> what the image's purpose is and which part of the image should be accessed
			// we don't need any mipmapping levels or multiple layers
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;
			// stereographic 3D application -> would have multiple layers (one for each eye)

			const VkResult result = vkCreateImageView(m_Device, &createInfo, nullptr, &m_SwapChainImageViews[i]);
			SS_CORE_ASSERT(result == VK_SUCCESS, "Failed to create image views!")
			}
		}
	}
}
