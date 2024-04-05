#include "pch.h"

#include "VulkanDevice.h"

#include <set>

#include "VulkanQueueFamilyIndices.h"
#include "VulkanSwapChain.h"

namespace Snowstorm
{
	VulkanDevice::VulkanDevice(const VkSurfaceKHR surface)
	{
		this->m_Surface = surface;
		PickPhysicalDevice();
		CreateLogicalDevice();
	}

	VulkanDevice::~VulkanDevice()
	{
		vkDestroyDevice(m_VkDevice, nullptr);
	}


	void VulkanDevice::PickPhysicalDevice()
	{
		const VkInstance vkInstance = VulkanInstance::GetInstance()->GetVulkanInstance();
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(vkInstance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		std::multimap<int, VkPhysicalDevice> candidates;

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(vkInstance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			int score = RateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));

			if (candidates.rbegin()->first > 0)
			{
				m_VkPhysicalDevice = candidates.rbegin()->second;
			}
			else
			{
				// TODO replace this with asserts
				throw std::runtime_error("failed to find a suitable GPU!");
			}
		}
	}

	void VulkanDevice::CreateLogicalDevice() 
	{
		const VulkanQueueFamilyIndices indices = VulkanQueueFamilyIndices::FindQueueFamilies(
			m_VkPhysicalDevice, m_Surface);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		constexpr float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			// we only need one for now - queue with graphics capabilities
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;

			// you don't really need more than one, since you can create all the command buffers on multiple threads
			// and submit them all at once without much overhead
			queueCreateInfo.queueCount = 1;

			// required even with single queue - for command scheduler - between 0.0 and 1.0
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}


		// next specify the set of m_VkDevice features that you'll be using
		VkPhysicalDeviceFeatures deviceFeatures{};
		// right now we don't need anything special, so leave it empty

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;

		// Next specify extensions/validation layers, but m_VkDevice specific this time
		// We only need to do this for backwards compatibility, though

		// The swap-chain extension though, has to be explicitly enabled at this point
		createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
		createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

		if (VulkanInstance::GetInstance()->EnabledValidationLayers())
		{
			std::vector<const char*> validationLayers = VulkanInstance::GetInstance()->GetValidationLayers();

			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		// Finally, instantiate the logical m_VkDevice
		if (vkCreateDevice(m_VkPhysicalDevice, &createInfo, nullptr, &m_VkDevice) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create logical m_VkDevice!");
		}

		// get the actual graphics queue
		// these two queue families most likely the same - the handles will have the same value
		vkGetDeviceQueue(m_VkDevice, indices.graphicsFamily.value(), 0, &m_GraphicsQueue);
		vkGetDeviceQueue(m_VkDevice, indices.presentFamily.value(), 0, &m_PresentQueue);
	}

	bool VulkanDevice::CheckDeviceExtensionSupport(const VkPhysicalDevice device) const
	{
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

		for (const auto& extension : availableExtensions)
		{
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
	}


	int VulkanDevice::RateDeviceSuitability(VkPhysicalDevice device) const
	{
		VkPhysicalDeviceProperties physicalDeviceProperties;
		VkPhysicalDeviceFeatures physicalDeviceFeatures;
		vkGetPhysicalDeviceProperties(device, &physicalDeviceProperties);
		vkGetPhysicalDeviceFeatures(device, &physicalDeviceFeatures);

		int score = 0;

		// Discrete GPUs have a significant performance advantage
		if (physicalDeviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
		{
			score += 1000;
		}

		// Maximum possible size of textures affects graphics quality
		score += static_cast<int>(physicalDeviceProperties.limits.maxImageDimension2D);

		const VulkanQueueFamilyIndices indices = VulkanQueueFamilyIndices::FindQueueFamilies(device, m_Surface);

		const bool extensionsSupported = CheckDeviceExtensionSupport(device);

		bool swapChainAdequate = false;
		if (extensionsSupported)
		{
			VulkanSwapChainSupportDetails swapChainSupport = VulkanSwapChain::QuerySwapChainSupport(device, m_Surface);
			swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
		}

		// Application can't function without geometry shaders or valid queue families
		// Or if swap chain is not supported (needed m_VkDevice extensions)
		if (!physicalDeviceFeatures.geometryShader || !indices.IsComplete() || !extensionsSupported || !
			swapChainAdequate)
		{
			return 0;
		}

		return score;
	}

}
