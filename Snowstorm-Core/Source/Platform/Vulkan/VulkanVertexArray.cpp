#include "pch.h"
#include "VulkanVertexArray.h"

#include <vulkan/vulkan_core.h>

namespace Snowstorm
{
	namespace
	{
		VkFormat ShaderDataTypeToVulkanFormat(const ShaderDataType type)
		{
			switch (type)
			{
			case ShaderDataType::Float: return VK_FORMAT_R32_SFLOAT;
			case ShaderDataType::Float2: return VK_FORMAT_R32G32_SFLOAT;
			case ShaderDataType::Float3: return VK_FORMAT_R32G32B32_SFLOAT;
			case ShaderDataType::Float4: return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ShaderDataType::Mat3: return VK_FORMAT_R32G32B32_SFLOAT;
			// Assuming each column of the matrix is a vec3
			case ShaderDataType::Mat4: return VK_FORMAT_R32G32B32A32_SFLOAT;
			case ShaderDataType::Int: return VK_FORMAT_R32_SINT;
			case ShaderDataType::Int2: return VK_FORMAT_R32G32_SINT;
			case ShaderDataType::Int3: return VK_FORMAT_R32G32B32_SINT;
			case ShaderDataType::Int4: return VK_FORMAT_R32G32B32A32_SINT;
			case ShaderDataType::Bool: return VK_FORMAT_R8_UINT;
			case ShaderDataType::None: break;
			}

			// Throw an assertion if an unknown ShaderDataType is encountered
			SS_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return VK_FORMAT_UNDEFINED;
		}
	}

	void VulkanVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		// TODO this only works for one vertex buffer - make it work for more in the future
		const auto& layout = vertexBuffer->GetLayout();

		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0; // only one binding for now
		bindingDescription.stride = layout.GetStride();
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		s_BindingDescription = bindingDescription;

		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		uint32_t offset = 0;
		uint32_t elementIndex = 0;
		for (const auto& element : layout)
		{
			VkVertexInputAttributeDescription attributeDescription{};
			attributeDescription.binding = 0; // assuming only one binding for now
			attributeDescription.location = elementIndex++; // location of the input in the vertex shader
			attributeDescription.format = ShaderDataTypeToVulkanFormat(element.Type); // 32-bit float component
			attributeDescription.offset = offset;
			offset += element.Size;

			attributeDescriptions.push_back(attributeDescription);
		}

		s_AttributeDescriptions = attributeDescriptions;

		m_VertexBuffers.push_back(vertexBuffer);
	}

	VkVertexInputBindingDescription VulkanVertexArray::GetBindingDescription()
	{
		// TODO fix this crap
		const BufferLayout layout{
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float2, "a_TexCoord"},
			{ShaderDataType::Float, "a_TextureIndex"},
			{ShaderDataType::Float, "a_TilingFactor"}
		};

		VkVertexInputBindingDescription bindingDescription{};
		bindingDescription.binding = 0; // only one binding for now
		bindingDescription.stride = layout.GetStride();
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescription;
	}

	std::vector<VkVertexInputAttributeDescription> VulkanVertexArray::GetAttributeDescriptions()
	{
		// TODO fix this crap
		const BufferLayout layout{
			{ShaderDataType::Float3, "a_Position"},
			{ShaderDataType::Float4, "a_Color"},
			{ShaderDataType::Float2, "a_TexCoord"},
			{ShaderDataType::Float, "a_TextureIndex"},
			{ShaderDataType::Float, "a_TilingFactor"}
		};


		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};

		uint32_t elementIndex = 0;
		for (const auto& element : layout)
		{
			VkVertexInputAttributeDescription attributeDescription{};
			attributeDescription.binding = 0; // assuming only one binding for now
			attributeDescription.location = elementIndex; // location of the input in the vertex shader
			attributeDescription.format = ShaderDataTypeToVulkanFormat(element.Type); 
			attributeDescription.offset = element.Offset;

			elementIndex++;

			attributeDescriptions.push_back(attributeDescription);
		}

		return attributeDescriptions;
	}
}
