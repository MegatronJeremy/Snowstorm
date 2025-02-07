#pragma once
#include "Snowstorm/Render/Framebuffer.hpp"


namespace Snowstorm
{
	class VulkanFrameBuffer final : public Framebuffer
	{
	public:
		explicit VulkanFrameBuffer(const FramebufferSpecification& spec);

		VulkanFrameBuffer(const VulkanFrameBuffer& other) = delete;
		VulkanFrameBuffer(VulkanFrameBuffer&& other) = delete;
		VulkanFrameBuffer& operator=(const VulkanFrameBuffer& other) = delete;
		VulkanFrameBuffer& operator=(VulkanFrameBuffer&& other) = delete;

		~VulkanFrameBuffer() override;

		void Invalidate();

		void Bind() override;
		void Unbind() override;

		void Resize(uint32_t width, uint32_t height) override;

		void Blit() override;

		uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; }

		const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachment = 0, m_DepthAttachment = 0;
		FramebufferSpecification m_Specification;
	};
}
