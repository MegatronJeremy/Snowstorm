#pragma once

#include <vulkan/vulkan_core.h>

#include "VulkanDevice.h"
#include "VulkanCommandBuffers.h"
#include "VulkanSwapChain.h"
#include "VulkanSyncObjects.h"
#include "Platform/Windows/WindowsWindow.h"
#include "Snowstorm/Render/GraphicsContext.hpp"
#include "glm/glm.hpp"

namespace Snowstorm
{
	class VulkanContext final : public GraphicsContext
	{
	public:
		explicit VulkanContext(GLFWwindow* windowHandle);
		~VulkanContext() override;

		VulkanContext(const VulkanContext& other) = delete;
		VulkanContext(VulkanContext&& other) = delete;
		VulkanContext& operator=(const VulkanContext& other) = delete;
		VulkanContext& operator=(VulkanContext&& other) = delete;

		void Init() override;
		void SwapBuffers() override;

		static void UpdateViewProjection(const glm::mat4& mat);
		static const UniformBufferObject& GetUniformBufferObject();

		static void SubmitUniformBufferObject(const UniformBufferObject& uniformBufferObject);

		static VulkanContext* Get();

		const Ref<VulkanSwapChain>& GetCurrentSwapChain();
		VkCommandBuffer GetCurrentFrameCommandBuffer() const;
		VkDescriptorSet GetCurrentFrameDescriptorSet() const;

	private:
		static void CreateUniformBuffers();
		void CreateDescriptorPool();
		void CreateDescriptorSets();

		void SubmitDrawCall(const Ref<VertexArray>& vertexArray, uint32_t indexCount) const;

		GLFWwindow* m_WindowHandle = VK_NULL_HANDLE;
		VkSurfaceKHR m_Surface = VK_NULL_HANDLE; // window m_Surface - tied to the GLFW window

		Ref<VulkanDevice> m_VulkanDevice;
		VkDevice m_Device = VK_NULL_HANDLE;
		VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

		Ref<VulkanCommandPool> m_CommandPool;
		Scope<VulkanCommandBuffers> m_CommandBuffers;

		Ref<VulkanSwapChain> m_SwapChain;

		std::vector<Scope<VulkanSemaphore>> m_ImageAvailableSemaphores;
		std::vector<Scope<VulkanSemaphore>> m_RenderFinishedSemaphores;
		std::vector<Scope<VulkanFence>> m_InFlightFences;

		bool m_FramebufferResized = false;

		VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
		VkQueue m_PresentQueue = VK_NULL_HANDLE;

		Scope<VulkanDescriptorPool> m_DescriptorPool;
		Scope<VulkanDescriptorSets> m_DescriptorSets;

		uint32_t m_CurrentImageIndex = 0;

		static inline uint32_t s_CurrentFrame = 0;

		static inline UniformBufferObject s_UniformBufferObject{};

		static inline std::vector<Ref<VulkanUniformBuffer>> s_UniformBuffers{};

		static inline VulkanContext* s_VulkanContext = nullptr;
	};
}
