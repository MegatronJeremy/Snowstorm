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
			if (s_Instance == nullptr)
			{
				s_Instance = new VulkanSwapChainQueue;
			}
			return s_Instance;
		}

		void AddVertexArray(const Ref<VertexArray>& vertexArray, const uint32_t indexCount);

		std::pair<Ref<VertexArray>, uint32_t> GetNextVertexArray();

		bool IsEmpty() const
		{
			return m_VertexArrays.empty();
		}

	private:
		VulkanSwapChainQueue() = default;

	private:
		static inline VulkanSwapChainQueue* s_Instance = nullptr;

		std::queue<std::pair<Ref<VertexArray>, uint32_t>> m_VertexArrays;
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

		void RecordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) const;

		static VulkanSwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

		static void SetViewport(const VkViewport& viewport)
		{
			s_Viewport = viewport;
		}

		static void SetClearValue(const VkClearValue& clearValue)
		{
			s_ClearValue = clearValue;
		}

	private:
		static VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

		static VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);

		static VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

		void CleanupSwapChain() const;

		void CreateSwapChain();

		void CreateImageViews();

		void CreateFramebuffers();

	private:
		static inline VkViewport s_Viewport;
		static inline VkClearValue s_ClearValue = {0.0f, 0.0f, 0.0f, 1.0f};

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
