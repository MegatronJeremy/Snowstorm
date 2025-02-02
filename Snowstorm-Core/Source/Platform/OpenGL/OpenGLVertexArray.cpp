#include "pch.h"
#include "OpenGLVertexArray.h"

#include <GL/glew.h>

namespace Snowstorm
{
	namespace
	{
		GLenum ShaderDataTypeToOpenGLBaseType(const ShaderDataType type)
		{
			switch (type)
			{
			case ShaderDataType::Float:
			case ShaderDataType::Float2:
			case ShaderDataType::Float3:
			case ShaderDataType::Float4:
			case ShaderDataType::Mat3:
			case ShaderDataType::Mat4: return GL_FLOAT;
			case ShaderDataType::Int:
			case ShaderDataType::Int2:
			case ShaderDataType::Int3:
			case ShaderDataType::Int4: return GL_INT;
			case ShaderDataType::Bool: return GL_BOOL;
			case ShaderDataType::None: break;
			}

			SS_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		SS_PROFILE_FUNCTION();

		glCreateVertexArrays(1, &m_RendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		SS_PROFILE_FUNCTION();

		glDeleteVertexArrays(1, &m_RendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		SS_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		SS_PROFILE_FUNCTION();

		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		SS_PROFILE_FUNCTION();

		SS_CORE_ASSERT(!vertexBuffer->GetLayout().GetElements().empty(), "Vertex buffer has no layout!");

		glBindVertexArray(m_RendererID);
		vertexBuffer->Bind();

		const auto& layout = vertexBuffer->GetLayout();

		for (const auto& element : layout)
		{
			const GLenum type = ShaderDataTypeToOpenGLBaseType(element.Type);
			const uint32_t componentCount = element.GetComponentCount();
			const bool isMatrix = (element.Type == ShaderDataType::Mat3 || element.Type == ShaderDataType::Mat4);
			const uint32_t rowCount = isMatrix ? componentCount : 1; // Matrices have multiple rows

			for (uint32_t i = 0; i < rowCount; i++)
			{
				glEnableVertexAttribArray(m_AttributeIndex);
				glVertexAttribPointer(
					m_AttributeIndex,
					componentCount,
					type,
					element.Normalized ? GL_TRUE : GL_FALSE,
					layout.GetStride(),
					reinterpret_cast<const void*>(element.Offset + i * sizeof(float) * rowCount));

				if (element.Instanced)
				{
					glVertexAttribDivisor(m_AttributeIndex, 1);
				}

				m_AttributeIndex++; // Increase attribute index for each row
			}
		}

		m_VertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		SS_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
		indexBuffer->Bind();

		m_IndexBuffer = indexBuffer;
	}

	void OpenGLVertexArray::SetInstanceDivisor(const uint32_t index, const uint32_t divisor) const
	{
		SS_PROFILE_FUNCTION();

		glBindVertexArray(m_RendererID);
		glVertexAttribDivisor(index, divisor);
	}
}
