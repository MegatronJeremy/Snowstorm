#include "pch.h"
#include "Platform/Vulkan/VulkanContext.h"

#include <GLFW/glfw3.h>

#include "VulkanInstance.h"

namespace Snowstorm
{
#define MAX_FRAMES_IN_FLIGHT 2

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

		m_VulkanCommandPool = CreateRef<VulkanCommandPool>(m_VulkanDevice->GetVkDevice(),
		                                                   m_VulkanDevice->GetVkPhysicalDevice(), m_Surface);

		VulkanInstance::GetInstance()->SetVulkanDevice(m_VulkanDevice);
		VulkanInstance::GetInstance()->SetVulkanCommandPool(m_VulkanCommandPool);

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

		SS_CORE_ASSERT(deviceMajor < requiredMajor || (deviceMajor == requiredMajor && deviceMinor < requiredMinor),
		               "Snowstorm requires at least Vulkan version 1.1!");
#endif
		// Create sync objects
		m_InFlightFences.resize(MAX_FRAMES_IN_FLIGHT);
		m_InFlightSemaphores.resize(MAX_FRAMES_IN_FLIGHT);

		// TODO create swap-chain here?
	}

	void VulkanContext::SwapBuffers()
	{
		SS_PROFILE_FUNCTION();

		// wait for the previous frame to finish
		m_InFlightFences[m_CurrentFrame].Wait();

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(m_Device, swapChain, UINT64_MAX,
		                                        imageAvailableSemaphores[currentFrame],
		                                        VK_NULL_HANDLE,
		                                        &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			// usually happens after a windows resize - the swap-chain has become incompatible with the surface
			// suboptimal - the surface properties are no longer matched exactly
			RecreateSwapChain();
			// recreate the swap chain and try drawing again in the next frame
			return;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		// Only reset the fence if we are submitting work
		m_InFlightFences[m_CurrentFrame].Reset();

		// record the command buffer
		vkResetCommandBuffer(commandBuffers[currentFrame], 0);
		// call this explicitly first to make sure it can be recorded to
		RecordCommandBuffer(commandBuffers[currentFrame], imageIndex);

		// submit the the command buffer
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		const VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
		constexpr VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores; // we want to wait until the image is available to start writing
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

		const VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		// submit the command buffer to the graphics queue
		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer!");
		}
		// the CPU will wait for the command buffer to finish executing before recording new commands into it

		// last step of drawing the frame - submitting the result back to the swap chain to have it show up on screen
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		const VkSwapchainKHR swapChains[] = {swapChain}; // almost always a single one
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		presentInfo.pResults = nullptr; // Optional - specifying an array of VkResult to check for every swap chain

		// submit a request for presentation
		result = vkQueuePresentKHR(presentQueue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
		{
			// same values with the same meaning as above
			// we want the best possible result here, so we will recreate even if suboptimal
			framebufferResized = false;
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

		// go to the next frame -> while the GPU is rendering we will prepare the next one on the CPU side without waiting
		m_CurrentFrame = (m_CurrentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void VulkanContext::RecreateSwapChain()
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(m_WindowHandle, &width, &height);
		while (width == 0 || height == 0)
		{
			// if width or height is 0 -> the window is minimized, so do nothing here
			glfwGetFramebufferSize(m_WindowHandle, &width, &height);
			glfwWaitEvents();
		}

		// we shouldn't touch resources that may still be in use
		vkDeviceWaitIdle(m_Device);

		// make sure the old versions of these objects are cleaned up first
		CleanupSwapChain();

		// we won't recreate the render pass, technically we may need to if moving to a different monitor,
		// but we won't take this into account

		CreateSwapChain();
		CreateImageViews();
		CreateFramebuffers();
	}


}
