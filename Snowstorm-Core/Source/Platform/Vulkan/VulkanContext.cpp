#include "pch.h"
#include "Platform/Vulkan/VulkanContext.h"

#include <GLFW/glfw3.h>

#include "VulkanInstance.h"

namespace Snowstorm
{
	class VulkanDevice;

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

		m_VulkanCommandPool = CreateRef<VulkanCommandPool>(m_VulkanDevice->GetVkDevice(),
		                                                   m_VulkanDevice->GetVkPhysicalDevice(), m_Surface);

		VulkanInstance::GetInstance()->SetVulkanDevice(m_VulkanDevice);
		VulkanInstance::GetInstance()->SetVulkanCommandPool(m_VulkanCommandPool);

		// TODO create swap-chain here?
	}

	void VulkanContext::SwapBuffers()
	{
		SS_PROFILE_FUNCTION();

		glfwSwapBuffers(m_WindowHandle);
	}
}
