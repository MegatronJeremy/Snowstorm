#pragma once

#include <vulkan/vulkan_core.h>

#include "VulkanDevice.h"
#include "VulkanCommandBuffers.h"
#include "VulkanSyncObjects.h"
#include "Platform/Windows/WindowsWindow.h"
#include "Snowstorm/Renderer/GraphicsContext.h"

namespace Snowstorm
{
	class VulkanContext final : public GraphicsContext
	{
	public:
		explicit VulkanContext(GLFWwindow* windowHandle);

		VulkanContext(const VulkanContext& other) = delete;
		VulkanContext(VulkanContext&& other) = delete;
		VulkanContext& operator=(const VulkanContext& other) = delete;
		VulkanContext& operator=(VulkanContext&& other) = delete;

		~VulkanContext() override;

		void Init() override;
		void SwapBuffers() override;

	private:
		void RecreateSwapChain();

	private:
		GLFWwindow* m_WindowHandle = VK_NULL_HANDLE;

		Ref<VulkanDevice> m_VulkanDevice;
		VkDevice m_Device = VK_NULL_HANDLE;

		Ref<VulkanCommandPool> m_VulkanCommandPool;

		VkSurfaceKHR m_Surface = VK_NULL_HANDLE; // window m_Surface - tied to the GLFW window

		std::vector<VulkanCommandBuffers> m_CommandBuffers;

		std::vector<VulkanSemaphore> m_InFlightSemaphores;
		std::vector<VulkanFence> m_InFlightFences;

		uint32_t m_CurrentFrame = 0;
	};
}

