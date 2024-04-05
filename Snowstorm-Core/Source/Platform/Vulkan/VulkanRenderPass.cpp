#include "pch.h"

#include "VulkanRenderPass.h"

namespace Snowstorm
{
	VulkanRenderPass::VulkanRenderPass(const VkDevice device, const VkFormat swapChainImageFormat)
		: m_Device(device)
	{
		// we need to specify how many color and depth buffers there will be, how many samples to use for each of them
		// and how their contents should be handled throughout the rendering operations

		// in our case - have a single color buffer attachment represented by one of the images from the swap chain
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // one sample - not doing anything with multisampling

		// what to do before and after rendering
		// clear the values to a constant at the start
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		// rendered contents stored to memory and can be read later
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		// we don't have a stencil buffer
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		// VkImage - textures and framebuffers (have a pixel format)
		// we don't care what previous layout the image was in (we are clearing it anyways)
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		// we want it to be ready for presentation using the swap chain after rendering
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		// single render pass - consists of multiple subpasses
		// grouping subpasses into one pass -> vulkan can optimize it using reordering of operations

		// for now, stick to a single subpass
		// every subpass references one or more attachments
		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0; // the index of the attachment
		// we use this attachment as a color buffer
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // explicitly a graphics subpass
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		// the index of the attachment is directly referenced in the fragment shader with layout(location = 0)

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		// Need to add render subpass dependencies
		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // special value - implicit subpass before/after render pass
		dependency.dstSubpass = 0; // our subpass, the first and only one

		// operations to wait on and the stages in which these operations occur
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;

		// the operations that should wait on this -> in the color attachment stage and involve writing of the color attachment 
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		// this prevents the transition from happening until it's actually necessary (and allowed)

		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		const VkResult result = vkCreateRenderPass(device, &renderPassInfo, nullptr, &m_RenderPass);
		SS_CORE_ASSERT(result == VK_SUCCESS, "Failed to create render pass!")
	}

	VulkanRenderPass::~VulkanRenderPass()
	{
		vkDestroyRenderPass(m_Device, m_RenderPass, nullptr);
	}
}
