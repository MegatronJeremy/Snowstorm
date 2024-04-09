#pragma once

#include <vulkan/vulkan_core.h>

#include "VulkanCommandBuffers.h"
#include "Snowstorm/Renderer/RendererAPI.h"

namespace Snowstorm
{
	class VulkanRendererAPI final : public RendererAPI
	{
	public:
		void Init() override;
		void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		void BeginRenderPass();
		void EndRenderPass();

		void SetClearColor(const glm::vec4& color) override;
		void Clear() override;

		void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount) override;

	private:
		Scope<VulkanCommandBuffers> m_VulkanCommandBuffer;
	};
}
