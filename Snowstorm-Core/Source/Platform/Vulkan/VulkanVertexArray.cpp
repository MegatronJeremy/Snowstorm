#include "pch.h"
#include "VulkanVertexArray.h"

#include "VulkanSwapChain.h"

namespace Snowstorm
{
	void VulkanVertexArray::Bind() const
	{
		VulkanSwapChainQueue::GetInstance()->AddVertexArray(*this);
	}
}
