#pragma once

#include "RendererAPI.h"

namespace Snowstorm
{
	class RenderCommand
	{
	public:
		static void Init()
		{
			s_RendererAPI->Init();
		}

		static void SetViewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
		{
			s_RendererAPI->SetViewport(x, y, width, height);
		}

		static void SetClearColor(const glm::vec4& color)
		{
			s_RendererAPI->SetClearColor(color);
		}

		static void Clear()
		{
			s_RendererAPI->Clear();
		}

		static void DrawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t count = 0)
		{
			s_RendererAPI->DrawIndexed(vertexArray, count);
		}

	private:
		static RendererAPI* s_RendererAPI;
	};
}
