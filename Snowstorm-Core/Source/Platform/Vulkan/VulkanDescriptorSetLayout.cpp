#include "pch.h"

#include "VulkanDescriptorSetLayout.h"

namespace Snowstorm
{
	VulkanDescriptorSetLayout::VulkanDescriptorSetLayout(const VkDevice device,
	                                                     const std::vector<VkDescriptorSetLayoutBinding>& bindings)
	                                               
		: m_Device(device)
	{
		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = bindings.size();
		layoutInfo.pBindings = bindings.data();

		const VkResult result = vkCreateDescriptorSetLayout(m_Device, &layoutInfo, nullptr, &m_DescriptorSetLayout);
		SS_CORE_ASSERT(result == VK_SUCCESS, "Failed to create descriptor set layout!");
	}

	VulkanDescriptorSetLayout::~VulkanDescriptorSetLayout()
	{
		vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, nullptr);
	}
}
