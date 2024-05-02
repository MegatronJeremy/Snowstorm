#include "pch.h"

#include "VulkanDescriptor.h"

namespace Snowstorm
{
    /////////////////////////////////////////////////////////////////////////////
    // Descriptor Set Layout ////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////

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

    /////////////////////////////////////////////////////////////////////////////
    // Descriptor Pool //////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////

    VulkanDescriptorPool::VulkanDescriptorPool(const VkDevice device, const VkDescriptorPoolSize& poolSize,
                                               const uint32_t maxSets)
        : m_Device(device)
    {
        VkDescriptorPoolCreateInfo poolInfo{};
        poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
        poolInfo.poolSizeCount = 1;
        poolInfo.pPoolSizes = &poolSize;
        poolInfo.maxSets = maxSets;

        const VkResult result = vkCreateDescriptorPool(device, &poolInfo, nullptr, &m_DescriptorPool);
        SS_CORE_ASSERT(result == VK_SUCCESS, "Failed to create descriptor pool!");
    }

    VulkanDescriptorPool::~VulkanDescriptorPool()
    {
        vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);
    }


    /////////////////////////////////////////////////////////////////////////////
    // Descriptor Sets //////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////

    VulkanDescriptorSets::VulkanDescriptorSets(const VkDevice device, const VkDescriptorPool descriptorPool,
                                               const uint32_t descriptorSetCount,
                                               const VkDescriptorSetLayout* descriptorSetLayouts)
        : m_Device(device)
    {
        VkDescriptorSetAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = descriptorSetCount;
        allocInfo.pSetLayouts = descriptorSetLayouts;

        m_DescriptorSets.resize(descriptorSetCount);
        const VkResult result = vkAllocateDescriptorSets(device, &allocInfo, m_DescriptorSets.data());
        SS_CORE_ASSERT(result == VK_SUCCESS, "Failed to allocate descriptor sets!");
    }

    void VulkanDescriptorSets::UpdateDescriptorSets(const std::vector<Ref<VulkanUniformBuffer>>& uniformBuffers) const
    {
        SS_CORE_ASSERT(uniformBuffers.size() == m_DescriptorSets.size(),
                       "Uniform buffer count does not match descriptor set count!");

        for (size_t i = 0; i < uniformBuffers.size(); i++)
        {
            VkDescriptorBufferInfo bufferInfo{};
            bufferInfo.buffer = *uniformBuffers[i];
            bufferInfo.offset = 0;
            bufferInfo.range = VK_WHOLE_SIZE;

            VkWriteDescriptorSet descriptorWrite{};
            descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            descriptorWrite.dstSet = m_DescriptorSets[i];
            descriptorWrite.dstBinding = 0;
            descriptorWrite.dstArrayElement = 0; // if we are using an array

            descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            descriptorWrite.descriptorCount = 1;

            descriptorWrite.pBufferInfo = &bufferInfo;
            descriptorWrite.pImageInfo = nullptr;
            descriptorWrite.pTexelBufferView = nullptr;

            vkUpdateDescriptorSets(m_Device, 1, &descriptorWrite, 0, nullptr);
        }
    }
}
