#pragma once

#include "Snowstorm/Render/Buffer.hpp"

namespace Snowstorm
{
	class OpenGLVertexBuffer final : public VertexBuffer
	{
	public:
		explicit OpenGLVertexBuffer(uint32_t size);
		OpenGLVertexBuffer(const void* data, uint32_t size);
		~OpenGLVertexBuffer() override;

		OpenGLVertexBuffer(const OpenGLVertexBuffer& other) = delete;
		OpenGLVertexBuffer(OpenGLVertexBuffer&& other) = delete;
		OpenGLVertexBuffer& operator=(const OpenGLVertexBuffer& other) = delete;
		OpenGLVertexBuffer& operator=(OpenGLVertexBuffer&& other) = delete;

		void SetData(const void* data, uint32_t size) override;
		void SetSubData(const void* data, uint32_t size, uint32_t offset) override;

		void Bind() const override;
		void Unbind() const override;

		const BufferLayout& GetLayout() const override { return m_Layout; }
		void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

	private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};

	class OpenGLIndexBuffer final : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(const uint32_t* indices, uint32_t count);
		~OpenGLIndexBuffer() override;

		OpenGLIndexBuffer(const OpenGLIndexBuffer& other) = delete;
		OpenGLIndexBuffer(OpenGLIndexBuffer&& other) = delete;
		OpenGLIndexBuffer& operator=(const OpenGLIndexBuffer& other) = delete;
		OpenGLIndexBuffer& operator=(OpenGLIndexBuffer&& other) = delete;

		void SetData(const void* data, uint32_t size) override;
		void SetSubData(const void* data, uint32_t size, uint32_t offset) override;

		void Bind() const override;
		void Unbind() const override;

		uint32_t GetCount() const override { return m_Count; }

	private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};
}
