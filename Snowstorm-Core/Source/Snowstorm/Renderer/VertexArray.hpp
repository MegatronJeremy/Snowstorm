#pragma once

#include <memory>

#include "Snowstorm/Renderer/Buffer.hpp"

namespace Snowstorm
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		virtual void SetInstanceDivisor(uint32_t index, uint32_t divisor) const = 0;

		[[nodiscard]] virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		[[nodiscard]] virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

		static Ref<VertexArray> Create();
	};
}
