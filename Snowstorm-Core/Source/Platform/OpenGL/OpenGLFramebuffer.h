#pragma once

#include "Snowstorm/Render/Framebuffer.hpp"

namespace Snowstorm
{
	class OpenGLFrameBuffer final : public Framebuffer
	{
	public:
		explicit OpenGLFrameBuffer(const FramebufferSpecification& spec);
		~OpenGLFrameBuffer() override;

		void Invalidate();

		void Bind() override;
		void Unbind() override;

		void Blit() override;

		void Resize(uint32_t width, uint32_t height) override;

		uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; }

		const FramebufferSpecification& GetSpecification() const override { return m_Specification; }

	private:
		uint32_t m_RendererID = 0;
		uint32_t m_ColorAttachment = 0, m_DepthAttachment = 0;
		FramebufferSpecification m_Specification;
	};
}
