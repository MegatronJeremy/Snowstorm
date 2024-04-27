#pragma once

#include <vulkan/vulkan_core.h>

#include "VulkanDevice.h"
#include "VulkanCommandBuffers.h"
#include "VulkanSwapChain.h"
#include "VulkanSyncObjects.h"
#include "Platform/Windows/WindowsWindow.h"
#include "Snowstorm/Renderer/GraphicsContext.h"
#include "glm/glm.hpp"

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

        static void UpdateViewProjection(const glm::mat4& mat);

    private:
        static void CreateUniformBuffers();
        void CreateDescriptorPool();
        void CreateDescriptorSets();

        GLFWwindow* m_WindowHandle = VK_NULL_HANDLE;
        VkSurfaceKHR m_Surface = VK_NULL_HANDLE; // window m_Surface - tied to the GLFW window

        Ref<VulkanDevice> m_VulkanDevice;
        VkDevice m_Device = VK_NULL_HANDLE;
        VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;

        Ref<VulkanCommandPool> m_CommandPool;
        Scope<VulkanCommandBuffers> m_CommandBuffers;

        Scope<VulkanSwapChain> m_SwapChain;

        std::vector<Scope<VulkanSemaphore>> m_ImageAvailableSemaphores;
        std::vector<Scope<VulkanSemaphore>> m_RenderFinishedSemaphores;
        std::vector<Scope<VulkanFence>> m_InFlightFences;


        bool m_FramebufferResized = false;

        VkQueue m_GraphicsQueue = VK_NULL_HANDLE;
        VkQueue m_PresentQueue = VK_NULL_HANDLE;

        Scope<VulkanDescriptorPool> m_DescriptorPool;
        Scope<VulkanDescriptorSets> m_DescriptorSets;

        static inline uint32_t s_CurrentFrame = 0;

        static inline struct UniformBufferObject
        {
            alignas(16) glm::mat4 viewProjection;
        } s_ViewProjection{};

        static inline std::vector<Ref<VulkanUniformBuffer>> s_UniformBuffers{};
    };
}
