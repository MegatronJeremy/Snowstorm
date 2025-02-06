#pragma once

#include "Snowstorm/Render/VertexArray.hpp"

namespace Snowstorm
{
	class OpenGLVertexArray final : public VertexArray
	{
	public:
		OpenGLVertexArray();
		~OpenGLVertexArray() override;

		void Bind() const override;
		void Unbind() const override;

		void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override;
		void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override;

		void SetInstanceDivisor(uint32_t index, uint32_t divisor) const override;

		[[nodiscard]] const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
		[[nodiscard]] const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

	private:
		uint32_t m_RendererID;
		uint32_t m_AttributeIndex{0};
		std::vector<Ref<VertexBuffer>> m_VertexBuffers;
		Ref<IndexBuffer> m_IndexBuffer;
	};

}
