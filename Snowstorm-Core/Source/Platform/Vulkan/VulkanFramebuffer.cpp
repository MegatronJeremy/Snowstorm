#include "pch.h"
#include "VulkanFramebuffer.h"

namespace Snowstorm
{
	namespace
	{
		constexpr uint32_t s_MaxFrameBufferSize = 8192;
	}

	// TODO see what to do with this

	VulkanFrameBuffer::VulkanFrameBuffer(const FramebufferSpecification& spec)
		: m_Specification(spec)
	{
		Invalidate();
	}

	VulkanFrameBuffer::~VulkanFrameBuffer()
	{
	}

	void VulkanFrameBuffer::Invalidate()
	{
	}

	void VulkanFrameBuffer::Bind()
	{
	}

	void VulkanFrameBuffer::Unbind()
	{
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
