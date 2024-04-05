#pragma once
#include <vulkan/vulkan_core.h>

#include "Platform/Windows/WindowsWindow.h"

namespace Snowstorm
{
	struct VulkanSwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanSwapChain
	{
	public:
		explicit VulkanSwapChain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface,
		                         GLFWwindow* window);

		VulkanSwapChain(const VulkanSwapChain& other) = delete;
		VulkanSwapChain(VulkanSwapChain&& other) = delete;
		VulkanSwapChain& operator=(const VulkanSwapChain& other) = delete;
		VulkanSwapChain& operator=(VulkanSwapChain&& other) = delete;

		~VulkanSwapChain();

		static VulkanSwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

	private:
		static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

		static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);


		void CreateImageViews();

	private:
		VkDevice m_Device = VK_NULL_HANDLE;
		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;

		std::vector<VkImage> m_SwapChainImages; // No cleanup code needed for this

		// creates a view for every image so we can use them as color targets
		std::vector<VkImageView> m_SwapChainImageViews;
	};
}
