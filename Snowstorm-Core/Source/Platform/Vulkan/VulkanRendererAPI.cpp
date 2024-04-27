#include "pch.h"
#include "Platform/Vulkan/VulkanRendererAPI.h"

#include <vulkan/vulkan_core.h>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "VulkanInstance.h"
#include "VulkanSwapChain.h"
#include "VulkanCommandPool.h"

namespace Snowstorm
{
	void VulkanRendererAPI::Init()
	{
		SS_PROFILE_FUNCTION();

		VkDevice device = VulkanInstance::GetInstance()->GetVulkanDevice()->GetVkDevice();
		VkCommandPool commandPool = *VulkanInstance::GetInstance()->GetVulkanCommandPool();

		m_VulkanCommandBuffer = CreateScope<VulkanCommandBuffers>(device, commandPool, 1);
	}

	void VulkanRendererAPI::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
	{
		// this is a delayed submission of a render command
		// Set viewport
		VkViewport viewport{};
		viewport.x = static_cast<float>(x);
		viewport.y = static_cast<float>(y);
		viewport.width = static_cast<float>(width);
		viewport.height = static_cast<float>(height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VulkanSwapChain::SetViewport(viewport);
	}

	void VulkanRendererAPI::SetClearColor(const glm::vec4& color)
	{
		const VkClearValue clearColor = {{color.r, color.g, color.b, color.a}};

		VulkanSwapChain::SetClearValue(clearColor);
	}

	void VulkanRendererAPI::Clear()
	{
		// TODO this should not actually exist - move it to swap buffers in OpenGl
	}

	void VulkanRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t indexCount)
	{
		// Draw indexed
		VulkanSwapChainQueue::GetInstance()->AddVertexArray(vertexArray, indexCount);
	}

	void VulkanRendererAPI::BeginFrame()
	{
		 // record the command buffer
		 // const VkCommandBuffer commandBuffer = (*m_CommandBuffers)[VulkanContext::s_CurrentFrame];
		 // vkResetCommandBuffer(commandBuffer, 0);
	}

	void VulkanRendererAPI::EndFrame()
	{
	}
}
