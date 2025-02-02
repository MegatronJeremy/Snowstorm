#pragma once

#include "Snowstorm/Renderer/UniformBuffer.hpp"

namespace Snowstorm
{
	class OpenGLUniformBuffer final : public UniformBuffer
	{
	public:
		OpenGLUniformBuffer(uint32_t size, uint32_t binding);
		~OpenGLUniformBuffer() override;

		void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;
		[[nodiscard]] uint32_t GetRendererID() const override { return m_RendererID; }

	private:
		uint32_t m_RendererID;
	};
}
