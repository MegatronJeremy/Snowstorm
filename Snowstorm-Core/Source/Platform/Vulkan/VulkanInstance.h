#pragma once
#include <vulkan/vulkan_core.h>

#include "VulkanCommandPool.h"
#include "VulkanDebugMessenger.h"

namespace Snowstorm
{
	class VulkanInstance
	{
		// Singleton class that holds the vulkan m_Instance
		// This m_Instance has all the app-related information stored in it
		// If multiple aps are required this singleton can be expanded to hold more instances
	public:
		static VulkanInstance* GetInstance()
		{
			if (m_Instance == nullptr)
			{
				m_Instance = new VulkanInstance;
			}
			return m_Instance;
		}

		VkInstance GetVulkanInstance() const
		{
			return m_VkInstance;
		}

		bool EnabledValidationLayers() const
		{
			return m_EnabledValidationLayers;
		}

		std::vector<const char*> GetValidationLayers() const
		{
			return m_ValidationLayers;
		}

		Ref<VulkanDevice> GetVulkanDevice() const
		{
			SS_CORE_ASSERT(m_VulkanDevice != nullptr, "Vulkan device not registered to Vulkan instance!");
			return m_VulkanDevice;
		}

		void SetVulkanDevice(const Ref<VulkanDevice>& vulkanDevice)
		{
			m_VulkanDevice = vulkanDevice;
		}

		Ref<VulkanCommandPool> GetVulkanCommandPool() const
		{
			SS_CORE_ASSERT(m_VulkanCommandPool != nullptr, "Vulkan command pool not registered to Vulkan instance!");
			return m_VulkanCommandPool;
		}

		void SetVulkanCommandPool(const Ref<VulkanCommandPool>& vulkanCommandPool)
		{
			m_VulkanCommandPool = vulkanCommandPool;
		}

	private:
		VulkanInstance();

		bool CheckValidationLayerSupport() const;
		std::vector<const char*> GetRequiredExtensions() const;

		void SetupDebugMessenger();

	private:
		static VulkanInstance* m_Instance;

		VkInstance m_VkInstance = nullptr;

		std::vector<const char*> m_ValidationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

		bool m_EnabledValidationLayers = SS_DEBUG;

		Scope<VulkanDebugMessenger> m_VulkanDebugMessenger = nullptr;

		Ref<VulkanDevice> m_VulkanDevice = nullptr;

		Ref<VulkanCommandPool> m_VulkanCommandPool = nullptr;
	};
}
