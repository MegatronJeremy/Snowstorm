#pragma once
#include "VulkanDevice.h"

namespace Snowstorm
{
	class VulkanGraphicsPipeline
	{
	public:
		explicit VulkanGraphicsPipeline(VkDevice device, VkRenderPass renderPass);

		VulkanGraphicsPipeline(const VulkanGraphicsPipeline& other) = delete;
		VulkanGraphicsPipeline(VulkanGraphicsPipeline&& other) = delete;
		VulkanGraphicsPipeline& operator=(const VulkanGraphicsPipeline& other) = delete;
		VulkanGraphicsPipeline& operator=(VulkanGraphicsPipeline&& other) = delete;

		~VulkanGraphicsPipeline();

	private:
		VkDevice m_Device;

		VkPipelineLayout m_PipelineLayout;

		VkRenderPass m_RenderPass;

		VkPipeline m_GraphicsPipeline;
	};
}
