#pragma once
#include <vulkan/vulkan_core.h>

namespace Snowstorm
{
	class VulkanDebugMessenger
	{
	public:
		explicit VulkanDebugMessenger(VkInstance instance);

		VulkanDebugMessenger(const VulkanDebugMessenger& other) = delete;
		VulkanDebugMessenger(VulkanDebugMessenger&& other) = delete;
		VulkanDebugMessenger& operator=(const VulkanDebugMessenger& other) = delete;
		VulkanDebugMessenger& operator=(VulkanDebugMessenger&& other) = delete;

		static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

		~VulkanDebugMessenger();

	private:
		VkInstance m_Instance = nullptr;
		VkDebugUtilsMessengerEXT m_VkDebugMessenger = nullptr;
	};
}
