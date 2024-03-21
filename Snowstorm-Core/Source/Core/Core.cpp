#include "Core.h"

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <vector>

namespace Core {
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
		VkInstance instance;
		GLFWwindow* window = nullptr;
	}

	void HelloTriangleApplication::Run() const
	{
		InitWindow();
		InitVulkan();
		MainLoop();
		Cleanup();
	}

	void HelloTriangleApplication::InitWindow() const
	{
		glfwInit();

		// force it to not use OpenGL (default)
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		// disabled for now - will be handled later
		glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

		// fourth param: optional monitor
		// fifth param: relevant to OpenGL only
		window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), "Vulkan", nullptr, nullptr);
	}

	void HelloTriangleApplication::CreateInstance()
	{
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
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char*> requiredExtensions;

		for (uint32_t i = 0; i < glfwExtensionCount; i++)
		{
			requiredExtensions.emplace_back(glfwExtensions[i]);
		}

		// check for available extensions
		uint32_t extensionCount = 0;
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> extensions(extensionCount);
		vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

		std::cout << "available extensions:\n";

		for (const auto& extension : extensions)
		{
			std::cout << "\t" << extension.extensionName << '\n';
			if (strcmp(extension.extensionName, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0)
			{
				// this is required now because of certain MacOS MoltenVK sdk-s
				requiredExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

				createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
			}
		}

		createInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
		createInfo.ppEnabledExtensionNames = requiredExtensions.data();

		// TODO leave these for later - enabled global validation layers
		createInfo.enabledLayerCount = 0;

		// pattern in Vulkan:
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

	void HelloTriangleApplication::InitVulkan()
	{
		CreateInstance();
	}

	void HelloTriangleApplication::MainLoop()
	{
		// event loop
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}
	}

	void HelloTriangleApplication::Cleanup()
	{
		// cleanup resources and terminate GLFW
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(window);

		glfwTerminate();
	}
#pragma endregion 
}
