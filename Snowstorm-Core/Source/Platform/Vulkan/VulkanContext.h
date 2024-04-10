#pragma once

#include <vulkan/vulkan_core.h>

#include "VulkanDevice.h"
#include "VulkanCommandBuffers.h"
#include "VulkanSwapChain.h"
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
		constexpr int MAX_FRAMES_IN_FLIGHT = 2;

		GLFWwindow* m_WindowHandle = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE; // window m_Surface - tied to the GLFW window

		Ref<VulkanDevice> m_VulkanDevice;
		VkDevice m_Device = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

		Ref<VulkanCommandPool> m_CommandPool;
		Scope<VulkanCommandBuffers> m_CommandBuffers;

		Scope<VulkanSwapChain> m_SwapChain;

		std::vector<VulkanSemaphore> m_ImageAvailableSemaphores;
		std::vector<VulkanSemaphore> m_RenderFinishedSemaphores;
		std::vector<VulkanFence> m_InFlightFences;

		uint32_t m_CurrentFrame = 0;

		bool m_FramebufferResized = false;

		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
		VkQueue m_PresentQueue = VK_NULL_HANDLE;
	};
}

