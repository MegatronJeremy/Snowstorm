#include "pch.h"
#include "Platform/Vulkan/VulkanBuffer.h"

namespace Snowstorm
{
	/////////////////////////////////////////////////////////////////////////////
	// Helper Functions //////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	namespace
	{
		void CreateBuffer(const VkDeviceSize size, const VkBufferUsageFlags usage,
		                  const VkMemoryPropertyFlags properties,
		                  VkBuffer& buffer, VkDeviceMemory& bufferMemory)
		{
			// helper function for creating a vertex buffer
			// last two parameters -> output parameters to write to
			VkBufferCreateInfo bufferInfo{};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;
			bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // will only be used from the graphics queue
			// flags - for sparse buffer memory, we will leave it at 0 for now

			if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create vertex buffer!");
			}

			// allocate the memory on the GPU for the buffer
			VkMemoryRequirements memRequirements;
			vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits,
			                                           properties);


			// TODO do not use vkAllocateMemory for every individual buffer
			// TODO max number of simultaneous memory allocations may be as low as 4096
			// TODO create a custom allocator that splits up a single allocation among different objects by using the offset param
			// TODO see: https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
			// TODO from: https://vulkan-tutorial.com/en/Vertex_buffers/Staging_buffer
			if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to allocate vertex buffer memory!");
			}

			// if everything was successful, we can now associate this memory with the buffer
			vkBindBufferMemory(device, buffer, bufferMemory, 0);
		}

		void CopyBuffer(const VkBuffer srcBuffer, const VkBuffer dstBuffer, const VkDeviceSize size)
		{
			// Memory transfer operations executed using command buffer, just like drawing commands
			// First allocate a temporary command buffer
			// TODO create a separate command pool for these kinds of short-lived buffers
			// TODO using VK_COMMAND_POOL_CREATE_TRANSIENT_BIT flag during command pool generation

			VkCommandBufferAllocateInfo allocInfo{};
			allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			allocInfo.commandPool = commandPool;
			allocInfo.commandBufferCount = 1;

			VkCommandBuffer commandBuffer;
			vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

			// Immediately record the command buffer
			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // good practice - tell the driver our intent

			vkBeginCommandBuffer(commandBuffer, &beginInfo);

			VkBufferCopy copyRegion{};
			copyRegion.srcOffset = 0; // Optional
			copyRegion.dstOffset = 0; // Optional
			copyRegion.size = size;

			// contents are transferred using this command
			vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

			// done with recording
			vkEndCommandBuffer(commandBuffer);

			// Now execute the command buffer to complete the transfer
			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &commandBuffer;

			vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);

			// TODO use a fence instead (vkWaitForFences) - we can schedule multiple transfers simultaneously
			// TODO and wait for all of them to complete, instead of doing this one task at a time
			// TODO see: https://vulkan-tutorial.com/en/Vertex_buffers/Staging_buffer
			vkQueueWaitIdle(graphicsQueue);

			// clean up the command buffer used for the transfer operation
			vkFreeCommandBuffers(device, commandPool, 1, &commandBuffer);
		}

	}

	/////////////////////////////////////////////////////////////////////////////
	// VertexBuffer /////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	VulkanVertexBuffer::VulkanVertexBuffer(uint32_t size)
	{
		SS_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	VulkanVertexBuffer::VulkanVertexBuffer(const float* vertices, const uint32_t size)
	{
		SS_PROFILE_FUNCTION();

		const VkDeviceSize bufferSize = sizeof(float) * size;

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		// we only use this to write from the CPU
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		             stagingBuffer, stagingBufferMemory);


		// now actually copy the vertex data to the buffer
		// by mapping the buffer memory into CPU accessible memory with vkMapMemory
		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, vertices, bufferSize);
		vkUnmapMemory(device, stagingBufferMemory);
		// the driver may not immediately copy the data into buffer memory -> because of caching for example
		// so we use memory that is host coherent: VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
		// OR we call vkFlushMappedMemoryRanged after writing to the mapped memory
		// AND call vkInvalidateMappedMemoryRanges before reading from the mapped memory

		// This buffer will be optimally used on the GPU - DEVICE_LOCAL_BIT in MEMORY
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
		             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		             vertexBuffer, vertexBufferMemory);

		// Now we need to transfer from the SRC buffer to the DST buffer
		CopyBuffer(stagingBuffer, vertexBuffer, bufferSize);

		// Finally, clean the staging buffer up
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	VulkanVertexBuffer::~VulkanVertexBuffer()
	{
		SS_PROFILE_FUNCTION();

		vkDestroyBuffer(device, vertexBuffer, nullptr);
		vkFreeMemory(device, vertexBufferMemory, nullptr);
	}

	void VulkanVertexBuffer::Bind() const
	{
		SS_PROFILE_FUNCTION();

		// if everything was successful, we can now associate this memory with the buffer
	}

	void VulkanVertexBuffer::Unbind() const
	{
		SS_PROFILE_FUNCTION();
	}

	void VulkanVertexBuffer::SetData(const void* data, uint32_t size)
	{
	}

	/////////////////////////////////////////////////////////////////////////////
	// IndexBuffer //////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	VulkanIndexBuffer::VulkanIndexBuffer(uint32_t* indices, const uint32_t count)
		: m_Count(count)
	{
		SS_PROFILE_FUNCTION();

		glCreateBuffers(1, &m_RendererID);

		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	VulkanIndexBuffer::~VulkanIndexBuffer()
	{
		SS_PROFILE_FUNCTION();

		glDeleteBuffers(1, &m_RendererID);
	}

	void VulkanIndexBuffer::Bind() const
	{
		SS_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void VulkanIndexBuffer::Unbind() const
	{
		SS_PROFILE_FUNCTION();

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

