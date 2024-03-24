#include "Core.h"

#include <iostream>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <algorithm>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

#include <iostream>
#include <map>
#include <set>
#include <vector>

namespace Core
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
		// ---------------
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

	void HelloTriangleApplication::MainLoop()
	{
		// event loop
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
		}
	}

	void HelloTriangleApplication::Cleanup() const
	{
		// cleanup resources and terminate GLFW
		for (const auto imageView : swapChainImageViews)
		{
			vkDestroyImageView(device, imageView, nullptr);
		}

		vkDestroySwapchainKHR(device, swapChain, nullptr);

		if (enableValidationLayers)
		{
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}

		vkDestroyDevice(device, nullptr);

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
