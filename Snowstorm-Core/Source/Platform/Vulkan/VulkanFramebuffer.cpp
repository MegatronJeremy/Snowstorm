#include "pch.h"
#include "VulkanFramebuffer.h"

namespace Snowstorm
{
	namespace
	{
		constexpr uint32_t s_MaxFrameBufferSize = 8192;
	}

	VulkanFrameBuffer::VulkanFrameBuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		Invalidate();
	}

	VulkanFrameBuffer::~VulkanFrameBuffer()
	{
		glDeleteFramebuffers(1, &m_RendererID);
		glDeleteTextures(1, &m_ColorAttachment);
		glDeleteTextures(1, &m_DepthAttachment);
	}

	void VulkanFrameBuffer::Invalidate()
	{
		if (m_RendererID)
		{
			glDeleteFramebuffers(1, &m_RendererID);
			glDeleteTextures(1, &m_ColorAttachment);
			glDeleteTextures(1, &m_DepthAttachment);
		}

		glCreateFramebuffers(1, &m_RendererID);
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_ColorAttachment);
		glBindTexture(GL_TEXTURE_2D, m_ColorAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, m_Specification.Width, m_Specification.Height);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachment, 0);

		glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
		glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Specification.Width, m_Specification.Height);

		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment, 0);

		SS_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
		               "Framebuffer is incomplete!");

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void VulkanFrameBuffer::Bind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
		glViewport(0, 0, m_Specification.Width, m_Specification.Height);
	}

	void VulkanFrameBuffer::Unbind()
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void VulkanFrameBuffer::Resize(const uint32_t width, const uint32_t height)
	{
		if (width == 0 || height == 0 || width > s_MaxFrameBufferSize || height > s_MaxFrameBufferSize)
		{
			SS_CORE_WARN("Attempted to resize framebuffer to {0}, {1}", width, height);
			return;
		}

		m_Specification.Width = width;
		m_Specification.Height = height;

		Invalidate();
	}
}
