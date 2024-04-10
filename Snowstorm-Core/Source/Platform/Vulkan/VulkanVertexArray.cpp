#include "pch.h"
#include "VulkanVertexArray.h"

#include "VulkanSwapChain.h"

namespace Snowstorm
{
	void VulkanVertexArray::Bind() const
	{
		VulkanSwapChainQueue::GetInstance()->AddVertexArray(*this);
	}

	std::vector<VkBuffer> VulkanVertexArray::GetVertexBuffers()
	{
	}

	VkBuffer VulkanVertexArray::GetIndexBuffer()
	{
		return *dynamic_cast<VulkanIndexBuffer*>(&*m_IndexBuffer);
	}
}
