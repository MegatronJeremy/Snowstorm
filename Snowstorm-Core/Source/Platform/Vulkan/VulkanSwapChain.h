#pragma once
#include <queue>
#include <vulkan/vulkan_core.h>

#include "VulkanGraphicsPipeline.h"
#include "VulkanRenderPass.h"
#include "VulkanVertexArray.h"
#include "Platform/Windows/WindowsWindow.h"

namespace Snowstorm
{
	struct VulkanSwapChainSupportDetails
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class VulkanSwapChainQueue
	{
	public:
		static VulkanSwapChainQueue* GetInstance()
		{
			if (m_Instance == nullptr)
			{
				m_Instance = new VulkanSwapChainQueue;
			}
			return m_Instance;
		}

		void AddVertexArray(const VulkanVertexArray& vertexArray);

		const VulkanVertexArray* GetNextVertexArray();

	private:
		VulkanSwapChainQueue() = default;

	private:
		static inline VulkanSwapChainQueue* m_Instance = nullptr;

		std::queue<const VulkanVertexArray*> m_VertexArrays;
	};

	class VulkanSwapChain
	{
	public:
		explicit VulkanSwapChain(VkPhysicalDevice physicalDevice, VkDevice device, VkSurfaceKHR surface,
		                         GLFWwindow* window);
		~VulkanSwapChain();

		VulkanSwapChain(const VulkanSwapChain& other) = delete;
		VulkanSwapChain(VulkanSwapChain&& other) = delete;
		VulkanSwapChain& operator=(const VulkanSwapChain& other) = delete;
		VulkanSwapChain& operator=(VulkanSwapChain&& other) = delete;

		operator VkSwapchainKHR() const
		{
			return m_SwapChain;
		}

		void RecreateSwapChain();

		void RecordCommandBuffer(const VkCommandBuffer commandBuffer, const uint32_t imageIndex) const;

		static VulkanSwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

	private:
		static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

		static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

		void CleanupSwapChain() const;

		void CreateSwapChain();

		void CreateImageViews();

		void CreateFramebuffers();

	private:
		VkDevice m_Device = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE;
		GLFWwindow* m_Window = nullptr;

		VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;

		VkFormat m_SwapChainImageFormat;
		VkExtent2D m_SwapChainExtent;

		std::vector<VkImage> m_SwapChainImages; // No cleanup code needed for this

		// creates a view for every image so we can use them as color targets
		std::vector<VkImageView> m_SwapChainImageViews;

		// we create a framebuffer for all of the images in the swap chain and use the one
		// that corresponds to the retrieved image at drawing time
		std::vector<VkFramebuffer> m_SwapChainFramebuffers;

		Scope<VulkanRenderPass> m_RenderPass;

		Scope<VulkanGraphicsPipeline> m_GraphicsPipeline;
	};
}
