#pragma once
#include "VulkanDevice.h"

namespace Snowstorm
{
	class VulkanDescriptorSetLayout
	{
	public:
		VulkanDescriptorSetLayout(VkDevice device, const std::vector<VkDescriptorSetLayoutBinding>& bindings);
		~VulkanDescriptorSetLayout();

		VulkanDescriptorSetLayout(const VulkanDescriptorSetLayout& other) = delete;
		VulkanDescriptorSetLayout(VulkanDescriptorSetLayout&& other) = delete;
		VulkanDescriptorSetLayout& operator=(const VulkanDescriptorSetLayout& other) = delete;
		VulkanDescriptorSetLayout& operator=(VulkanDescriptorSetLayout&& other) = delete;

		operator VkDescriptorSetLayout() const
		{
			return m_DescriptorSetLayout;
		}

	private:
		VkDevice m_Device;
		VkDescriptorSetLayout m_DescriptorSetLayout;

	};
}
