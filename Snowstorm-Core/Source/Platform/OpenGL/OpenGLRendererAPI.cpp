#include "pch.h"
#include "OpenGLRendererAPI.h"

#include <GL/glew.h>

namespace Snowstorm
{
	void OpenGLRendererAPI::Init()
	{
		SS_PROFILE_FUNCTION();

		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS); // Default depth testing

		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK); // Cull back faces
		glFrontFace(GL_CCW); // Counter-clockwise is front-facing

		// Enable Blending for transparency (Optional)
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		// For debugging
		// glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

		// Enable seamless cubemap filtering (Optional)
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	}

	void OpenGLRendererAPI::SetViewport(const uint32_t x, const uint32_t y, const uint32_t width, const uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t indexCount)
	{
		const uint32_t count = indexCount == 0 ? vertexArray->GetIndexBuffer()->GetCount() : indexCount;
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}

	void OpenGLRendererAPI::DrawIndexedInstanced(const Ref<VertexArray>& vertexArray, const uint32_t indexCount,
	                                             const uint32_t instanceCount)
	{
		const uint32_t count = indexCount == 0 ? vertexArray->GetIndexBuffer()->GetCount() : indexCount;
		glDrawElementsInstanced(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr, instanceCount);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
