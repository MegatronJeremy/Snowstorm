#pragma once
#include <vulkan/vulkan_core.h>

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

		Scope<VulkanDebugMessenger> m_DebugMessenger = nullptr;
	};
}
