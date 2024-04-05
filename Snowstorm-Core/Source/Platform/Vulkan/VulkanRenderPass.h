#pragma once
#include "VulkanDevice.h"

namespace Snowstorm
{
	class VulkanRenderPass
	{
	public:
		VulkanRenderPass(VkDevice device, VkFormat swapChainImageFormat);

		VulkanRenderPass(const VulkanRenderPass& other) = delete;
		VulkanRenderPass(VulkanRenderPass&& other) = delete;
		VulkanRenderPass& operator=(const VulkanRenderPass& other) = delete;
		VulkanRenderPass& operator=(VulkanRenderPass&& other) = delete;

		~VulkanRenderPass();

	private:
		VkDevice m_Device = VK_NULL_HANDLE;
		VkRenderPass m_RenderPass = VK_NULL_HANDLE;
		
	};
}
