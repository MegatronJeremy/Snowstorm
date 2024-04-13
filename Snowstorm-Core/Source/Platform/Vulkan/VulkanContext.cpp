#include "pch.h"
#include "Platform/Vulkan/VulkanContext.h"

#include <GLFW/glfw3.h>

#include "VulkanInstance.h"

namespace Snowstorm
{
	VulkanContext::VulkanContext(GLFWwindow* windowHandle)
		: m_WindowHandle(windowHandle)
	{
		SS_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	VulkanContext::~VulkanContext()
	{
		SS_PROFILE_FUNCTION();

		vkDestroySurfaceKHR(VulkanInstance::GetInstance()->GetVulkanInstance(), m_Surface, nullptr);
	}

	void VulkanContext::Init()
	{
		SS_PROFILE_FUNCTION();

		// NOTE: this call is completely platform agnostic!
		const VkResult result = glfwCreateWindowSurface(VulkanInstance::GetInstance()->GetVulkanInstance(),
		                                                m_WindowHandle, nullptr,
		                                                &m_Surface);
		SS_CORE_ASSERT(result == VK_SUCCESS, "Failed to create window m_Surface!")

		m_VulkanDevice = CreateRef<VulkanDevice>(m_Surface);
		m_Device = m_VulkanDevice->GetVkDevice();
		m_PhysicalDevice = m_VulkanDevice->GetVkPhysicalDevice();

		m_CommandPool = CreateRef<VulkanCommandPool>(m_Device, m_PhysicalDevice, m_Surface);

		VulkanInstance::GetInstance()->SetVulkanDevice(m_VulkanDevice);
		VulkanInstance::GetInstance()->SetVulkanCommandPool(m_CommandPool);

		VkPhysicalDeviceProperties deviceProperties;
		vkGetPhysicalDeviceProperties(VulkanInstance::GetInstance()->GetVulkanDevice()->GetVkPhysicalDevice(),
		                              &deviceProperties);

		std::cout << "Vendor ID: " << deviceProperties.vendorID << std::endl;
		std::cout << "Device Name: " << deviceProperties.deviceName << std::endl;

		// Vulkan's version numbers are encoded, so we need to decode them to get the major, minor, and patch version.
		const uint32_t major = VK_VERSION_MAJOR(deviceProperties.driverVersion);
		const uint32_t minor = VK_VERSION_MINOR(deviceProperties.driverVersion);
		const uint32_t patch = VK_VERSION_PATCH(deviceProperties.driverVersion);
		SS_CORE_INFO("	Driver Version: {0}.{1}.{2}", major, minor, patch);

#ifdef SS_ENABLE_ASSERTS
		constexpr uint32_t requiredMajor = 1;
		constexpr uint32_t requiredMinor = 1;
		const uint32_t deviceMajor = VK_VERSION_MAJOR(deviceProperties.apiVersion);
		const uint32_t deviceMinor = VK_VERSION_MINOR(deviceProperties.apiVersion);

		SS_CORE_ASSERT(deviceMajor >= requiredMajor && (deviceMajor != requiredMajor || deviceMinor >= requiredMinor),
		               "Snowstorm requires at least Vulkan version 1.1!");
#endif

		// Assign queues
		m_GraphicsQueue = m_VulkanDevice->GetVkGraphicsQueue();
		m_PresentQueue = m_VulkanDevice->GetVkPresentQueue();

		// Create swap chain
		m_SwapChain = CreateScope<VulkanSwapChain>(m_PhysicalDevice, m_Device, m_Surface, m_WindowHandle);

		m_CommandPool = CreateRef<VulkanCommandPool>(m_Device, m_PhysicalDevice, m_Surface);

		m_CommandBuffers = CreateScope<VulkanCommandBuffers>(m_Device, static_cast<VkCommandPool>(*m_CommandPool),
		                                                     MAX_FRAMES_IN_FLIGHT);

		// Create sync objects
		for (int i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			m_ImageAvailableSemaphores.emplace_back(CreateScope<VulkanSemaphore>(m_Device));
			m_RenderFinishedSemaphores.emplace_back(CreateScope<VulkanSemaphore>(m_Device));
			m_InFlightFences.emplace_back(CreateScope<VulkanFence>(m_Device, true));
		}
	}

	void VulkanContext::SwapBuffers()
	{
		SS_PROFILE_FUNCTION();

		// wait for the previous frame to finish
		m_InFlightFences[m_CurrentFrame]->Wait();

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(m_Device, *m_SwapChain, UINT64_MAX,
		                                        *m_ImageAvailableSemaphores[m_CurrentFrame],
		                                        VK_NULL_HANDLE,
		                                        &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			// usually happens after a windows resize - the swap-chain has become incompatible with the surface
			// suboptimal - the surface properties are no longer matched exactly
			m_SwapChain->RecreateSwapChain();
			// recreate the swap chain and try drawing again in the next frame
			return;
		}

		SS_CORE_ASSERT(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR,
		               "Failed to acquire swap chain image!");

		// Only reset the fence if we are submitting work
		// TODO MOVE THIS TO RENDER API DRAW CALL
		m_InFlightFences[m_CurrentFrame]->Reset(); // manually reset

		// record the command buffer
		const VkCommandBuffer commandBuffer = (*m_CommandBuffers)[m_CurrentFrame];
		vkResetCommandBuffer(commandBuffer, 0);
		// call this explicitly first to make sure it can be recorded to
		m_SwapChain->RecordCommandBuffer((*m_CommandBuffers)[m_CurrentFrame], imageIndex);

		// submit the command buffer
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		const VkSemaphore waitSemaphores[] = {*m_ImageAvailableSemaphores[m_CurrentFrame]};
		constexpr VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores; // we want to wait until the image is available to start writing
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		const VkSemaphore signalSemaphores[] = {*m_RenderFinishedSemaphores[m_CurrentFrame]};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		// submit the command buffer to the graphics queue
		result = vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, *m_InFlightFences[m_CurrentFrame]);
		SS_CORE_ASSERT(result == VK_SUCCESS, "Failed to submit draw command buffer!");
		// TODO MOVE THIS TO RENDER API DRAW CALL

		// the CPU will wait for the m_Command buffer to finish executing before recording new commands into it

		// last step of drawing the frame - submitting the result back to the swap chain to have it show up on screen
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		const VkSwapchainKHR swapChains[] = {*m_SwapChain}; // almost always a single one
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		presentInfo.pResults = nullptr; // Optional - specifying an array of VkResult to check for every swap chain

		// submit a request for presentation
		// this is basically swap buffers
		result = vkQueuePresentKHR(m_PresentQueue, &presentInfo);

		const bool recreateSwapchainCondition = result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR ||
			m_FramebufferResized;
		if (recreateSwapchainCondition)
		{
			// same values with the same meaning as above
			// we want the best possible result here, so we will recreate even if suboptimal
			m_FramebufferResized = false;
			m_SwapChain->RecreateSwapChain();
		}

		SS_CORE_ASSERT(recreateSwapchainCondition || result == VK_SUCCESS, "Failed to present swap chain image!");

		// go to the next frame -> while the GPU is rendering we will prepare the next one on the CPU side without waiting
		m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}
}
