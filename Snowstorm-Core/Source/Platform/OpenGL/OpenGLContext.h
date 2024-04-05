#pragma once

#include "Snowstorm/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Snowstorm
{
	class OpenGLContext final : public GraphicsContext
	{
	public:
		~OpenGLContext() override = default;

		explicit OpenGLContext(GLFWwindow* windowHandle);

		void Init() override;
		void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}
