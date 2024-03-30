#include "pch.h"
#include "Core.h"

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <algorithm>
#include <fstream>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <map>
#include <set>
#include <vector>

#include "Snowstorm/Renderer/Buffer.h"

namespace Snowstorm
{
#pragma region BasicTests
	void PrintHelloWorld()
	{
		std::cout << "Hello World!\n";
		std::cin.get();
	}

	void RenderingTest()
	{
		glfwInit();

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		GLFWwindow* window = glfwCreateWindow(800, 600, "Vulkan window", nullptr, nullptr);

		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::cout << extensionCount << " extensions supported\n";

		glm::mat4 matrix;
		glm::vec4 vec;
		auto test = matrix * vec;

		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}

		glfwDestroyWindow(window);

		glfwTerminate();
	}
#pragma endregion

#pragma region HelloTriangleApplication
	namespace
	{
#ifdef SS_DEBUG
		bool enableValidationLayers = true;
#else
		bool enableValidationLayers = false;
#endif

		VkSurfaceKHR surface; // window surface - tied to the GLFW window
		// has a platform specific extension - INCLUDED IN glfwGetRequiredInstanceExtensions!

		// We need to enable the swap chain - so first check if it is supported
		const std::vector<const char*> deviceExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};

		GLFWwindow* window = nullptr;
	}

	namespace
	{
		// -----------------------------------DEBUG MESSENGER--------------------------------------
		VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
			VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
			VkDebugUtilsMessageTypeFlagsEXT messageType,
			const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
			void* pUserData)
		{
			// override debug callback for vulkan API
			// if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			// {
			// Message is important enough to show
			// }
			std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

			// always return VK_FALSE,
			// VK_ERROR_VALIDATION_FAILED_EXT used only to test validation layers themselves
			return VK_FALSE;
		}

		void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
		{
			createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;

			// specify all the severities the callback should be called for
			createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;

			// similarly - filter the types of messages your callback is notified about
			createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT
				| VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;

			createInfo.pfnUserCallback = DebugCallback;

			// can pass a pointer to the HelloTriangleApplication, for example
			createInfo.pUserData = nullptr; // optional
		}

		// proxy function - stub needs to be loaded in since it is an extension
		VkResult CreateDebugUtilsMessengerEXT(const VkInstance instance,
		                                      const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		                                      const VkAllocationCallbacks* pAllocator,
		                                      VkDebugUtilsMessengerEXT* pDebugMessenger)
		{
			if (const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
					vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
				func != nullptr)
			{
				return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
			}
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}

		void DestroyDebugUtilsMessengerEXT(const VkInstance instance,
		                                   const VkDebugUtilsMessengerEXT debugMessenger,
		                                   const VkAllocationCallbacks* pAllocator)
		{
			if (const auto func = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
					vkGetInstanceProcAddr(
						instance, "vkDestroyDebugUtilsMessengerEXT"));
				func != nullptr)
			{
				func(instance, debugMessenger, pAllocator);
			}
		}

		// -----------------------------------SWAP CHAIN-----------------------------------------
		struct SwapChainSupportDetails
		{
			VkSurfaceCapabilitiesKHR capabilities;
			std::vector<VkSurfaceFormatKHR> formats;
			std::vector<VkPresentModeKHR> presentModes;
		};

		SwapChainSupportDetails QuerySwapChainSupport(const VkPhysicalDevice device)
		{
			SwapChainSupportDetails details;

			// device and surface -> core components of the swap chain
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);

			// query available surface formats
			uint32_t formatCount;
			vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

			if (formatCount != 0)
			{
				details.formats.resize(formatCount); // vector resize to hold all the formats
				vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount,
				                                     details.formats.data());
			}

			// query available presentation modes
			uint32_t presentModeCount;
			vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

			if (presentModeCount != 0)
			{
				details.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount,
				                                          details.presentModes.data());
			}

			return details;
		}

		VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats)
		{
			for (const auto& availableFormat : availableFormats)
			{
				// this is optimal -> we always want sRGB
				if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB &&
					availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				{
					return availableFormat;
				}
			}

			// if it fails, ok to settle with the first format available
			return availableFormats[0];
		}

		VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes)
		{
			// VK_PRESENT_MODE_FIFO_KHR -> always guaranteed -> this is vertical sync (use on mobile phones)
			// VK_PRESENT_MODE_MAILBOX_KHR -> triple buffering -> good if energy expenditure is not an issue

			for (const auto& availablePresentMode : availablePresentModes)
			{
				if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					return availablePresentMode;
				}
			}

			return VK_PRESENT_MODE_FIFO_KHR;
		}

		VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities)
		{
			// swap extent - resolution of the swap chain -> (almost always) equal to the
			// resolution of the window we are drawing in -> in pixels
			// width and height -> in currentExtent
			// we can differ here sometimes -> set the currentExtent to the max value of uint32_t
			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				return capabilities.currentExtent; // we want to match it here
			}

			// certain windows managers allow differing values
			int width, height;
			glfwGetFramebufferSize(window, &width, &height); // query the resolution of the window in pixel

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			// match against the minimum and maximum image extent
			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width,
			                                capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height,
			                                 capabilities.maxImageExtent.height);

			return actualExtent;
		}


		// -----------------------------------DEVICE PICKER--------------------------------------
		QueueFamilyIndices FindQueueFamilies(const VkPhysicalDevice device)
		{
			QueueFamilyIndices indices;

			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

			int i = 0;
			for (const auto& queueFamily : queueFamilies)
			{
				// we need to find at least one queue family that support graphics commands and presentation commands
				if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				{
					indices.graphicsFamily = i;
				}

				VkBool32 presentSupport = false;
				vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);

				if (presentSupport)
				{
					indices.presentFamily = i;
				}

				// if found both break immediately
				if (indices.IsComplete())
				{
					break;
				}

				i++;
			}


			return indices;
		}

		bool CheckDeviceExtensionSupport(const VkPhysicalDevice device)
		{
			uint32_t extensionCount;
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

			std::vector<VkExtensionProperties> availableExtensions(extensionCount);
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

			std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

			for (const auto& extension : availableExtensions)
			{
				requiredExtensions.erase(extension.extensionName);
			}

			return requiredExtensions.empty();
		}

		int RateDeviceSuitability(const VkPhysicalDevice device)
		{
			VkPhysicalDeviceProperties deviceProperties;
			VkPhysicalDeviceFeatures deviceFeatures;
			vkGetPhysicalDeviceProperties(device, &deviceProperties);
			vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

			int score = 0;

			// Discrete GPUs have a significant performance advantage
			if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			{
				score += 1000;
			}

			// Maximum possible size of textures affects graphics quality
			score += static_cast<int>(deviceProperties.limits.maxImageDimension2D);

			const QueueFamilyIndices indices = FindQueueFamilies(device);

			const bool extensionsSupported = CheckDeviceExtensionSupport(device);

			bool swapChainAdequate = false;
			if (extensionsSupported)
			{
				SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(device);
				swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentModes.empty();
			}

			// Application can't function without geometry shaders or valid queue families
			// Or if swap chain is not supported (needed device extensions)
			if (!deviceFeatures.geometryShader || !indices.IsComplete() || !extensionsSupported || !swapChainAdequate)
			{
				return 0;
			}

			return score;
		}
	}

	namespace
	{
		VkInstance instance;

		VkDebugUtilsMessengerEXT debugMessenger;

		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

		VkDevice device; // logical device - to interface with the physical device

		VkQueue graphicsQueue; // implicitly destroyed when the device is destroyed
		VkQueue presentQueue;

		// --------------- swap-chain
		VkSwapchainKHR swapChain;

		std::vector<VkImage> swapChainImages; // No cleanup code needed for this

		// creates a view for every image so we can use them as color targets
		std::vector<VkImageView> swapChainImageViews;

		VkFormat swapChainImageFormat;
		VkExtent2D swapChainExtent;

		// attachments in the render pass will be bound to the framebuffer
		// we create a framebuffer for all of the images in the swap chain and use the one
		// that corresponds to the retrieved image at drawing time
		std::vector<VkFramebuffer> swapChainFramebuffers;

		// --------------- graphics pipeline
		VkRenderPass renderPass;
		VkPipelineLayout pipelineLayout;
		VkPipeline graphicsPipeline;
		// ---------------

		// --------------- command buffer
		VkCommandPool commandPool;
		std::vector<VkCommandBuffer> commandBuffers;
		// ---------------

		// --------------- synchronization
		std::vector<VkSemaphore> imageAvailableSemaphores;
		std::vector<VkSemaphore> renderFinishedSemaphores;
		std::vector<VkFence> inFlightFences;

		bool framebufferResized = false;
		// ---------------

		// --------------- drawing
		uint32_t currentFrame = 0;
		// ---------------

		// --------------- various buffers
		// TODO make this not hardcoded
		const std::vector<Vertex> vertices = {
			{{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
			{{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}},
			{{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}}
		};

		// uint16_t -> if using less than 65535 unique vertices
		// but we can also use uint32_t
		const std::vector<uint16_t> indices = {
			0, 1, 2, 2, 3, 0
		};

		VkBuffer vertexBuffer;
		VkDeviceMemory vertexBufferMemory;
		VkBuffer indexBuffer;
		VkDeviceMemory indexBufferMemory;
		// ---------------
	}

	namespace
	{
		// -----------------------------------GRAPHICS PIPELINE--------------------------------------
		std::vector<char> ReadFile(const std::string& filename)
		{
			// ate: start reading at the end of the file
			std::ifstream file(filename, std::ios::ate | std::ios::binary);

			if (!file.is_open())
			{
				throw std::runtime_error("failed to open file!");
			}

			// we can use the end position to tell the size of the buffer
			const size_t fileSize = file.tellg();
			std::vector<char> buffer(fileSize);

			// seek back to the beginning and load the data
			file.seekg(0);
			file.read(buffer.data(), static_cast<std::streamsize>(fileSize));

			file.close();

			return buffer;
		}

		VkShaderModule CreateShaderModule(const std::vector<char>& code)
		{
			// std::vector -> the default allocator ensures that the data satisfies the worst case alignment requirements
			VkShaderModuleCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			createInfo.codeSize = code.size();
			createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

			VkShaderModule shaderModule;
			if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS)
			{
				// TODO replace this with asserts
				throw std::runtime_error("failed to create shader module!");
			}

			return shaderModule;
		}

		// -----------------------------------COMMAND BUFFER--------------------------------------
		void RecordCommandBuffer(const VkCommandBuffer commandBuffer, const uint32_t imageIndex)
		{
			// writes the commands from the command buffer to the swap chain image tied to the index 

			VkCommandBufferBeginInfo beginInfo{};
			beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			beginInfo.flags = 0; // Optional - not relevant right now
			beginInfo.pInheritanceInfo = nullptr; // Optional - only relevant for secondary command buffers

			if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to begin recording command buffer!");
			}

			// begin render pass
			VkRenderPassBeginInfo renderPassInfo{};
			renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			renderPassInfo.renderPass = renderPass;
			renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];

			// define the size of the render area - match the size of the attachment
			renderPassInfo.renderArea.offset = {0, 0};
			renderPassInfo.renderArea.extent = swapChainExtent;

			// clear values to use for VK_ATTACHMENT_LOAD_OP_CLEAR
			constexpr VkClearValue clearColor = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
			renderPassInfo.clearValueCount = 1;
			renderPassInfo.pClearValues = &clearColor;

			// begin the render pass
			vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
			// inline - embedded in the primary command buffer itself, with no secondary command buffers 

			// bind the graphics pipeline
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

			// viewport and scissor states are dynamic - set them here
			// viewport - the region of the framebuffer that the output will be rendered to
			// always (0, 0) to (width, height)
			// here we define the transformation from the image to the framebuffer
			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = static_cast<float>(swapChainExtent.width);
			viewport.height = static_cast<float>(swapChainExtent.height);
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f; // range of depth values used for the framebuffer
			vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

			// next define the scissor rectangle -> basically works like a filter
			VkRect2D scissor{};
			scissor.offset = {0, 0}; // we want the whole image
			scissor.extent = swapChainExtent;
			vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

			// bind the vertex buffer during rendering operations
			vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

			const VkBuffer vertexBuffers[] = {vertexBuffer};
			constexpr VkDeviceSize offsets[] = {0}; // offsets to start reading vertex data from
			// bind vertex buffers to bindings
			vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

			// we have to bind the index buffer as well
			vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
			// You can only have a SINGLE index buffer! not possible to use different indices for each vertex attribute

			// finally, issue the draw command
			// vkCmdDraw(commandBuffer, vertices.size(), 1, 0, 0);

			// change the drawing command to drawIndexed to utilize the index buffer
			vkCmdDrawIndexed(commandBuffer, indices.size(), 1, 0, 0, 0);
			// first index - offset in the index buffer
			// 1 instance - we are not using instancing
			// TODO allocate multiple resources like buffers from a single memory allocation
			// TODO and store multiple buffers into a single VkBuffer and use offsets in commands like vkCmdBindVertexBuffers
			// TODO the data is more cache friendly this way! (this is known as aliasing)

			// firstVertex: lowest value of gl_VertexIndex
			// firstInstance: offset for instanced rendering, lowest value of gl_InstanceIndex

			// end the render pass and finish recording the command buffer
			vkCmdEndRenderPass(commandBuffer);

			if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to record command buffer!");
			}
		}

		// -----------------------------------BUFFER FUNCTIONS--------------------------------------
		uint32_t FindMemoryType(const uint32_t typeFilter, const VkMemoryPropertyFlags properties)
		{
			// graphics cards can offer different types of memory to allocate from
			VkPhysicalDeviceMemoryProperties memProperties;
			vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

			// right now we'll only concern ourselves with the type of memory and not the heap it comes from
			// we also need to be able to write our vertex data to that memory, so we check additional bits
			// so we check if ALL the properties are present
			// TODO this impacts performance, check it out again later
			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			{
				if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				{
					return i;
				}
			}

			throw std::runtime_error("failed to find suitable memory type!");
		}

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

		void CopyBuffer(const VkBuffer srcBuffer, const VkBuffer dstBuffer, VkDeviceSize size)
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


	void HelloTriangleApplication::Run() 
	{
		InitWindow();
		InitVulkan();
		MainLoop();
		Cleanup();
	}

	namespace
	{
		// TODO move this somewhere else
		void FramebufferResizeCallback(GLFWwindow* window, int width, int height)
		{
			auto app = static_cast<HelloTriangleApplication*>(glfwGetWindowUserPointer(window));
			// TODO app->framebufferResized = true
			framebufferResized = true;
		}
	}


	void HelloTriangleApplication::InitWindow() 
	{
		glfwInit();

		// force it to not use OpenGL (default)
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		// set the window to not be resizable - optional
		// glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		// fourth param: optional monitor
		// fifth param: relevant to OpenGL only
		window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), "Vulkan", nullptr, nullptr);
		glfwSetWindowUserPointer(window, this);
		glfwSetFramebufferSizeCallback(window, FramebufferResizeCallback);
	}

	void HelloTriangleApplication::CreateInstance() const
	{
		// TODO make this an assert as well
		if (enableValidationLayers && !CheckValidationLayerSupport())
		{
			throw std::runtime_error("validation layers requested, but not available!");
		}

		// useful info that the driver can use to optimize our application
		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;
		// appInfo.pNext -> points to an extension in the future

		// this tells the Vulkan driver which global extensions and validation layers to use
		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		// platform agnostic - need an extension to interface with the window system
		auto requiredExtensions = GetRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();

		// check for other available extensions
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

		std::cout << "available extensions:\n";

		for (const auto& extension : availableExtensions)
		{
			std::cout << "\t" << extension.extensionName << '\n';
			if (strcmp(extension.extensionName, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0)
			{
				// this is required now because of certain MacOS MoltenVK sdk-s
				requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

				createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
			}
		}

		// handle validation layers
		// and add additional debug messenger just for vkCreateInstance

		// placed outside if to make sure it is valid up to vkCreateInstance
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			PopulateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = &debugCreateInfo; // add it to pNext field
		}
		else
		{
			createInfo.enabledLayerCount = 0;

			createInfo.pNext = nullptr;
		}

		// common pattern in Vulkan:
		// 1. Pointer to struct with creation info
		// 2. Pointer to custom allocator callbacks
		// 3. Pointer to the variable that stores the handle to the new object
		
		const VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);
		if (result != VK_SUCCESS)
		{
			// TODO replace this with assertions
			throw std::runtime_error("failed to create instance!");
		}
	}

	void HelloTriangleApplication::InitVulkan() const
	{
		CreateInstance();
		SetupDebugMessenger();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateSwapChain();
		CreateImageViews();
		CreateRenderPass();
		CreateGraphicsPipeline();
		CreateFramebuffers();
		CreateCommandPool();
		CreateVertexBuffer();
		CreateIndexBuffer();
		CreateCommandBuffers();
		CreateSyncObjects();
	}

	void HelloTriangleApplication::PickPhysicalDevice()
	{
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0)
		{
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		std::multimap<int, VkPhysicalDevice> candidates;

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices)
		{
			int score = RateDeviceSuitability(device);
			candidates.insert(std::make_pair(score, device));

			if (candidates.rbegin()->first > 0)
			{
				physicalDevice = candidates.rbegin()->second;
			}
			else
			{
				// TODO replace this with asserts
				throw std::runtime_error("failed to find a suitable GPU!");
			}
		}
	}


	void HelloTriangleApplication::SetupDebugMessenger() const
	{
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		PopulateDebugMessengerCreateInfo(createInfo);
		

		// this is only for our specific Vulkan instance and its layers
		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS)
		{
			// TODO assert here
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	void HelloTriangleApplication::CreateSurface()
	{
		// NOTE: this call is completely platform agnostic!
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create window surface!");
		}
	}


	void HelloTriangleApplication::CreateLogicalDevice() const
	{
		const QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

		std::set<uint32_t> uniqueQueueFamilies = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		constexpr float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies)
		{
			// we only need one for now - queue with graphics capabilities
			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;

			// you don't really need more than one, since you can create all the command buffers on multiple threads
			// and submit them all at once without much overhead
			queueCreateInfo.queueCount = 1;

			// required even with single queue - for command scheduler - between 0.0 and 1.0
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
		}


		// next specify the set of device features that you'll be using
		VkPhysicalDeviceFeatures deviceFeatures{};
		// right now we don't need anything special, so leave it empty

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = &deviceFeatures;

		// Next specify extensions/validation layers, but device specific this time
		// We only need to do this for backwards compatibility, though

		// The swap-chain extension though, has to be explicitly enabled at this point
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

		if (enableValidationLayers)
		{
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else
		{
			createInfo.enabledLayerCount = 0;
		}

		// Finally, instantiate the logical device
		if (vkCreateDevice(physicalDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create logical device!");
		}

		// get the actual graphics queue
		// these two queue families most likely the same - the handles will have the same value
		vkGetDeviceQueue(device, indices.graphicsFamily.value(), 0, &graphicsQueue);
		vkGetDeviceQueue(device, indices.presentFamily.value(), 0, &presentQueue);
	}

	void HelloTriangleApplication::CreateSwapChain()
	{
		const SwapChainSupportDetails swapChainSupport = QuerySwapChainSupport(physicalDevice);

		const VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(swapChainSupport.formats);
		const VkPresentModeKHR presentMode = ChooseSwapPresentMode(swapChainSupport.presentModes);
		const VkExtent2D extent = ChooseSwapExtent(swapChainSupport.capabilities);

		// recommended to request at least one more image than the minimum -> else we have to wait for internal operations
		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;

		// also do not exceed the maximum number of images while doing this (0 is a special value - there is no max)
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount)
		{
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		// Now create the actual swap chain
		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;

		// first specify the surface the swap chain should be tied to
		createInfo.surface = surface;

		// details of the swap chain images go next
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;

		// amount of layers each image consists of -> always 1 unless with a stereoscopic 3D app
		createInfo.imageArrayLayers = 1;

		// what kind of operations we'll use the images for -> we render directly to them here
		// also possible: rendering images to a separate image first, then do post-processing (VK_IMAGE_USAGE_TRANSFER_DST_BIT), then transfer to a swap chain image
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		// Next specify how to handle swap chain images used across multiple queue families
		// This is the case when the graphics queue family is different from the presentation queue
		const QueueFamilyIndices indices = FindQueueFamilies(physicalDevice);

		const uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};

		// draw from the graphics queue -> then submit to the presentation queue
		if (indices.graphicsFamily != indices.presentFamily)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			// images can be used concurrently across multiple queue families
			// TODO we will do ownership transfer later

			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			// best performance - explicit ownership transfer

			createInfo.queueFamilyIndexCount = 0; // Optional
			createInfo.pQueueFamilyIndices = nullptr; // Optional
		}

		// if we want to do a pre-transform, like rotating by 90 degrees, etc.
		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;

		// specifies if the alpha channel should be used for blending with other windows in the window system
		// we almost always want to ignore this
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

		// if clipped is VK_TRUE -> we don't care about the color of pixels that are obscured (if another window is in front for example)
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		// old swap chain -> with Vulkan it is possible that another swap chain becomes invalid/unoptimized (if the window is resized)
		// in this case, the swap chain needs to be recreated from scratch!
		// TODO we will do this in the future, for now, only assume one swap chain
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create swap chain!");
		}

		// we only specified a minimum number of images -> the implementation is allowed to create a swap chain with more
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(device, swapChain, &imageCount, swapChainImages.data());

		// store these for later
		swapChainImageFormat = surfaceFormat.format;
		swapChainExtent = extent;
	}

	void HelloTriangleApplication::CreateImageViews()
	{
		swapChainImageViews.resize(swapChainImages.size());

		for (size_t i = 0; i < swapChainImages.size(); i++)
		{
			VkImageViewCreateInfo createInfo{};
			createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			createInfo.image = swapChainImages[i];
			createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			createInfo.format = swapChainImageFormat;

			// stick to the default mappings -> we can do stuff like monochrome here instead
			createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

			// subresource range -> what the image's purpose is and which part of the image should be accessed
			// we don't need any mipmapping levels or multiple layers
			createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			createInfo.subresourceRange.baseMipLevel = 0;
			createInfo.subresourceRange.levelCount = 1;
			createInfo.subresourceRange.baseArrayLayer = 0;
			createInfo.subresourceRange.layerCount = 1;
			// stereographic 3D application -> would have multiple layers (one for each eye)

			if (vkCreateImageView(device, &createInfo, nullptr, &swapChainImageViews[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create image views!");
			}
		}
	}

	void HelloTriangleApplication::CreateRenderPass()
	{
		// we need to specify how many color and depth buffers there will be, how many samples to use for each of them
		// and how their contents should be handled throughout the rendering operations

		// in our case - have a single color buffer attachment represented by one of the images from the swap chain
		VkAttachmentDescription colorAttachment{};
		colorAttachment.format = swapChainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT; // one sample - not doing anything with multisampling

		// what to do before and after rendering
		// clear the values to a constant at the start
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		// rendered contents stored to memory and can be read later
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;

		// we don't have a stencil buffer
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		// VkImage - textures and framebuffers (have a pixel format)
		// we don't care what previous layout the image was in (we are clearing it anyways)
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		// we want it to be ready for presentation using the swap chain after rendering
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		// single render pass - consists of multiple subpasses
		// grouping subpasses into one pass -> vulkan can optimize it using reordering of operations

		// for now, stick to a single subpass
		// every subpass references one or more attachments
		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = 0; // the index of the attachment
		// we use this attachment as a color buffer
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass{};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // explicitly a graphics subpass
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		// the index of the attachment is directly referenced in the fragment shader with layout(location = 0)

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;

		// Need to add render subpass dependencies
		VkSubpassDependency dependency{};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // special value - implicit subpass before/after render pass
		dependency.dstSubpass = 0; // our subpass, the first and only one

		// operations to wait on and the stages in which these operations occur
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;

		// the operations that should wait on this -> in the color attachment stage and involve writing of the color attachment 
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		// this prevents the transition from happening until it's actually necessary (and allowed)

		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		if (vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create render pass!");
		}
	}

	void HelloTriangleApplication::CreateGraphicsPipeline()
	{
		// load the code
		const auto vertShaderCode = ReadFile("Assets/Shaders/vert.spv");
		const auto fragShaderCode = ReadFile("Assets/Shaders/frag.spv");

		// create the shaders with a wrapper
		const VkShaderModule vertShaderModule = CreateShaderModule(vertShaderCode);
		const VkShaderModule fragShaderModule = CreateShaderModule(fragShaderCode);

		// assign the actual shaders to a specific pipeline stage
		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT; // enum for each programmable stage
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main"; // entry point (we can combine multiple shaders into a single file this way)
		// pSpecializationInfo -> you can specify values for shader constants

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

		// describe the format of the vertex data that will be passed to the vertex shader
		// bindings: spacing between data and whether the data is per-vertex or per-instance
		// attribute descriptions: type of the attributes passed to the vertex shader, which binding
		// to load them from and at which offset
		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		auto bindingDescription = Vertex::GetBindingDescription();
		auto attributeDescriptions = Vertex::GetAttributeDescriptions();

		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

		// next define the topology - we intend to draw triangles so stick to this structure
		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST; // triangle from every 3 vertices without reuse
		inputAssembly.primitiveRestartEnable = VK_FALSE;
		// with an element buffer - specify indices to use yourself (reuse vertices - optimization)

		
		// setup the dynamic state, we can change and must specify these properties without recreating the pipeline from scratch
		// at runtime we can change the viewport and teh scissor state
		const std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};

		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();

		// after this only need to specify their count at pipeline creation time
		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;
		// the actual viewport(s) and scissor rectangle(s) will then later be set up at drawing time
		// using multiple viewports and scissor rectangles requires enabling a GPU feature

		// rasterizer - turns geometry into fragments to be colored by the fragment shader
		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;

		rasterizer.depthClampEnable = VK_FALSE;
		// if set to VK_TRUE, the fragments beyond the near and far planes are clamped as opposed to discarded
		// useful in some place like shadow maps (need to enable a GPU feature for this)

		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		// if set to true, the geometry never passes through the rasterizer stage (disables output to framebuffer)

		rasterizer.polygonMode = VK_POLYGON_MODE_FILL; // fill the area of the polygon with fragments (fill)
		// determines how fragments are generated for geometry
		// any other mode requires a GPU feature

		rasterizer.lineWidth = 1.0f;
		// thickness of line in terms of number of fragments
		// any line thicker than 1.0f requires you to enable the wideLines GPU feature

		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		// cullMode - type of face culling to use
		// frontFace - vertex order for faces to be considered front-facing

		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f; // optional
		rasterizer.depthBiasClamp = 0.0f; // optional
		rasterizer.depthBiasSlopeFactor = 0.0f; // optional
		// altering depth values by adding a constant or biasing based on the fragment slope - we won't use
		// sometimes used for shadow mapping

		// multisampling - one way to perform anti-aliasing
		// combines the fragment shader results of multiple polygons that rasterize to the same pixel
		// mostly along the edges
		// not expensive - doesn't need to run the fragment shader multiple times if only one polygon maps to a pixel
		// enabling it requires enabling a GPU feature
		// TODO we will revisit this later
		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f; // Optional
		multisampling.pSampleMask = nullptr; // Optional
		multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
		multisampling.alphaToOneEnable = VK_FALSE; // Optional

		// if using depth/stencil buffer, need to configure the depth and stencil tests
		// using VkPipelineDepthStencilStateCreateInfo, we don't have one right now, so we can pass nullptr

		// Color blending
		// after a fragment shader returns a color, it needs to be combined with the color that is already in the
		// framebuffer -> color blending
		// Two ways: 1. mix the old and new value; 2. combine using bitwise operation 
		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		// there is another global struct (this one is tied to framebuffer), but we have only one framebuffer
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
			VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		// write mask - to determine which channels are passed through -  in this case, all of them

		// alpha blending:
		// finalColor.rgb = newAlpha * newColor + (1 - newAlpha) * oldColor;
		// finalColor.a = newAlpha * a;
		colorBlendAttachment.blendEnable = VK_TRUE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

		// global blend factors/constants
		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE; // if we were to use bitwise combinations
		colorBlending.logicOp = VK_LOGIC_OP_COPY; // Optional
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment; // array of attachments here for all the framebuffers
		colorBlending.blendConstants[0] = 0.0f; // Optional
		colorBlending.blendConstants[1] = 0.0f; // Optional
		colorBlending.blendConstants[2] = 0.0f; // Optional
		colorBlending.blendConstants[3] = 0.0f; // Optional

		// uniform values - to be specified by the VkPipelineLayout object
		// TODO we will be using this at a later stage, keep it empty for now
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0; // Optional
		pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
		pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional
		// push constants -> another way of passing dynamic values to shaders

		if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create pipeline layout!");
		}

		// finally, create the graphics pipeline
		VkGraphicsPipelineCreateInfo pipelineInfo{};

		// reference the array of structs
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;

		// reference all the structures describing the fixed-function state
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = nullptr; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;

		// reference the pipeline layout - Vulkan handle
		pipelineInfo.layout = pipelineLayout;

		// reference to the render pass and the index of the subpass where this pipeline is used
		pipelineInfo.renderPass = renderPass;
		pipelineInfo.subpass = 0;
		// we can also use other render passes with this pipeline IF COMPATIBLE
		// we won't be using this now

		// we can derive from an existing pipeline -> less expensive than creating from scratch
		// right now there is only a single pipeline
		// only used if the VK_PIPELINE_CREATE_DERIVATIVE_BIT is set in VkGraphicsPipelineCreateInfo
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; // Optional
		pipelineInfo.basePipelineIndex = -1; // Optional

		// this function can actually create multiple pipelines immediately
		// pipeline cache - for storing and reusing data relevant to pipeline creation across multiple calls
		// TODO we will use a pipeline cache later
		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) !=
			VK_SUCCESS)
		{
			throw std::runtime_error("failed to create graphics pipeline!");
		}


		vkDestroyShaderModule(device, fragShaderModule, nullptr);
		vkDestroyShaderModule(device, vertShaderModule, nullptr);
	}

	void HelloTriangleApplication::CreateFramebuffers()
	{
		swapChainFramebuffers.resize(swapChainImageViews.size());

		for (size_t i = 0; i < swapChainImageViews.size(); i++)
		{
			const VkImageView attachments[] = {
				swapChainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass; // you can only use a framebuffer with the compatible render pass
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments; // binds it to the pAttachment of the render pass
			framebufferInfo.width = swapChainExtent.width;
			framebufferInfo.height = swapChainExtent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
	}

	void HelloTriangleApplication::CreateCommandPool()
	{
		const QueueFamilyIndices queueFamilyIndices = FindQueueFamilies(physicalDevice);

		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		// command buffers can be rerecorded individually
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		// we are recording commands for drawing
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

		if (vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to create command pool!");
		}
	}

	void HelloTriangleApplication::CreateVertexBuffer()
	{
		const VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

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
		memcpy(data, vertices.data(), bufferSize);
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

		// Finally, clean the buffer up
		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	void HelloTriangleApplication::CreateIndexBuffer()
	{
		// This looks basically the same as creating a vertex buffer - well, it actually is the same
		// TODO make this a single function -> CreateBuffer 
		const VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		             VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
		             stagingBuffer, stagingBufferMemory);

		void* data;
		vkMapMemory(device, stagingBufferMemory, 0, bufferSize, 0, &data);
		memcpy(data, indices.data(), bufferSize);
		vkUnmapMemory(device, stagingBufferMemory);

		// This is a difference -> VK_BUFFER_USAGE_INDEX_BUFFER_BIT -> a single boolean!
		CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
		             VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
		             indexBuffer, indexBufferMemory);

		CopyBuffer(stagingBuffer, indexBuffer, bufferSize);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	void HelloTriangleApplication::CreateCommandBuffers()
	{
		commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		// primary: can be submitted to a queue for execution, but cannot be called from other command buffers
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

		if (vkAllocateCommandBuffers(device, &allocInfo, commandBuffers.data()) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allocate command buffers!");
		}
	}

	void HelloTriangleApplication::CreateSyncObjects()
	{
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO; // current version

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // create signaled for the first frame

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			if (vkCreateSemaphore(device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS
				||
				vkCreateSemaphore(device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS
				||
				vkCreateFence(device, &fenceInfo, nullptr, &inFlightFences[i]) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create semaphore!");
			}
		}
	}

	void HelloTriangleApplication::MainLoop()
	{
		// event loop
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
			DrawFrame();
		}

		// wait for the device to finish execution before destroying resources
		vkDeviceWaitIdle(device);
	}

	void HelloTriangleApplication::DrawFrame()
	{
		// wait for the previous frame to finish
		vkWaitForFences(device, 1, &inFlightFences[currentFrame], VK_TRUE, UINT64_MAX);

		uint32_t imageIndex;
		VkResult result = vkAcquireNextImageKHR(device, swapChain, UINT64_MAX,
		                                        imageAvailableSemaphores[currentFrame],
		                                        VK_NULL_HANDLE,
		                                        &imageIndex);

		if (result == VK_ERROR_OUT_OF_DATE_KHR)
		{
			// usually happens after a windows resize - the swap-chain has become incompatible with the surface
			// suboptimal - the surface properties are no longer matched exactly
			RecreateSwapChain();
			// recreate the swap chain and try drawing again in the next frame
			return;
		}
		if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
		{
			throw std::runtime_error("failed to acquire swap chain image!");
		}

		// Only reset the fence if we are submitting work
		vkResetFences(device, 1, &inFlightFences[currentFrame]); // manually reset

		// record the command buffer
		vkResetCommandBuffer(commandBuffers[currentFrame], 0);
		// call this explicitly first to make sure it can be recorded to
		RecordCommandBuffer(commandBuffers[currentFrame], imageIndex);

		// submit the the command buffer
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		const VkSemaphore waitSemaphores[] = {imageAvailableSemaphores[currentFrame]};
		constexpr VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores; // we want to wait until the image is available to start writing
		submitInfo.pWaitDstStageMask = waitStages;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffers[currentFrame];

		const VkSemaphore signalSemaphores[] = {renderFinishedSemaphores[currentFrame]};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		// submit the command buffer to the graphics queue
		if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, inFlightFences[currentFrame]) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to submit draw command buffer!");
		}
		// the CPU will wait for the command buffer to finish executing before recording new commands into it

		// last step of drawing the frame - submitting the result back to the swap chain to have it show up on screen
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		const VkSwapchainKHR swapChains[] = {swapChain}; // almost always a single one
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;

		presentInfo.pResults = nullptr; // Optional - specifying an array of VkResult to check for every swap chain

		// submit a request for presentation
		result = vkQueuePresentKHR(presentQueue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || framebufferResized)
		{
			// same values with the same meaning as above
			// we want the best possible result here, so we will recreate even if suboptimal
			framebufferResized = false;
			RecreateSwapChain();
		}
		else if (result != VK_SUCCESS)
		{
			throw std::runtime_error("failed to present swap chain image!");
		}

		// go to the next frame -> while the GPU is rendering we will prepare the next one on the CPU side without waiting
		currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
	}

	void HelloTriangleApplication::CleanupSwapChain()
	{
		for (const auto& swapChainFramebuffer : swapChainFramebuffers)
		{
			vkDestroyFramebuffer(device, swapChainFramebuffer, nullptr);
		}

		for (const auto& swapChainImageView : swapChainImageViews)
		{
			vkDestroyImageView(device, swapChainImageView, nullptr);
		}

		vkDestroySwapchainKHR(device, swapChain, nullptr);
	}

	void HelloTriangleApplication::RecreateSwapChain()
	{
		int width = 0, height = 0;
		glfwGetFramebufferSize(window, &width, &height);
		while (width == 0 || height == 0)
		{
			// if width or height is 0 -> the window is minimized, so do nothing here
			glfwGetFramebufferSize(window, &width, &height);
			glfwWaitEvents();
		}

		// we shouldn't touch resources that may still be in use
		vkDeviceWaitIdle(device);

		// make sure the old versions of these objects are cleaned up first
		CleanupSwapChain();

		// we won't recreate the render pass, technically we may need to if moving to a different monitor,
		// but we won't take this into account

		CreateSwapChain();
		CreateImageViews();
		CreateFramebuffers();
	}

	void HelloTriangleApplication::Cleanup()
	{
		// cleanup resources and terminate GLFW
		CleanupSwapChain();

		// the various buffers do not depend on the swap-chain
		vkDestroyBuffer(device, indexBuffer, nullptr);
		vkFreeMemory(device, indexBufferMemory, nullptr);

		vkDestroyBuffer(device, vertexBuffer, nullptr);
		vkFreeMemory(device, vertexBufferMemory, nullptr);

		vkDestroyPipeline(device, graphicsPipeline, nullptr);
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);

		vkDestroyRenderPass(device, renderPass, nullptr);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
		{
			vkDestroySemaphore(device, imageAvailableSemaphores[i], nullptr);
			vkDestroySemaphore(device, renderFinishedSemaphores[i], nullptr);
			vkDestroyFence(device, inFlightFences[i], nullptr);
		}

		vkDestroyCommandPool(device, commandPool, nullptr);

		vkDestroyDevice(device, nullptr);

		if (enableValidationLayers)
		{
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}

		// make sure the surface is destroyed before the window
		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);

		glfwTerminate();
	}

	bool HelloTriangleApplication::CheckValidationLayerSupport() const
	{
		// check if all of the requested layers are available
		uint32_t layerCount;
		vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

		for (const char* layerName : validationLayers)
		{
			bool layerFound = false;

			// TODO kinda crap complexity
			for (const auto& layerProperties : availableLayers)
			{
				if (strcmp(layerName, layerProperties.layerName) == 0)
				{
					layerFound = true;
					break;
				}
			}

			if (!layerFound)
			{
				return false;
			}
		}

		return true;
	}

	std::vector<const char*> HelloTriangleApplication::GetRequiredExtensions() const
	{
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

		if (enableValidationLayers)
		{
			// add external debug message utilities - enables us to filter messages
			requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return requiredExtensions;
	}
#pragma endregion
}
