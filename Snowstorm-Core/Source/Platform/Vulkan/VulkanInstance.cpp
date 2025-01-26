#include "pch.h"

#include "VulkanInstance.h"

#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

namespace Snowstorm
{
    Ref<VulkanDevice> VulkanInstance::GetVulkanDevice() const
    {
        SS_CORE_ASSERT(m_VulkanDevice != nullptr, "Vulkan device not registered to Vulkan instance!");
        return m_VulkanDevice;
    }

    void VulkanInstance::SetVulkanDevice(const Ref<VulkanDevice>& vulkanDevice)
    {
        m_VulkanDevice = vulkanDevice;
    }

    Ref<VulkanCommandPool> VulkanInstance::GetVulkanCommandPool() const
    {
        SS_CORE_ASSERT(m_VulkanCommandPool != nullptr, "Vulkan command pool not registered to Vulkan instance!");
        return m_VulkanCommandPool;
    }

    void VulkanInstance::SetVulkanCommandPool(const Ref<VulkanCommandPool>& vulkanCommandPool)
    {
        m_VulkanCommandPool = vulkanCommandPool;
    }

    VulkanInstance::VulkanInstance()
    {
        SS_PROFILE_FUNCTION();

        SS_CORE_ASSERT(!m_EnabledValidationLayers || CheckValidationLayerSupport(),
                       "Validation layers requested, but not available!")

        // useful info that the driver can use to optimize our application
        VkApplicationInfo appInfo{};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "APP";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "SNOWSTORM";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        // appInfo.pNext -> points to an extension in the future

        // this tells the Vulkan driver which global extensions and validation layers to use
        VkInstanceCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;

        // platform agnostic - need an extension to interface with the window system
        auto enabledExtensions = GetRequiredExtensions();

        // check for other available extensions
        uint32_t extensionCount = 0;
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

        std::vector<VkExtensionProperties> availableExtensions(extensionCount);
        vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, availableExtensions.data());

        // Check for additional extension
        for (const auto& extension : availableExtensions)
        {
            // Check for macOS MoltenVK sdk (required now)
            if (strcmp(extension.extensionName, VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME) == 0)
            {
                enabledExtensions.emplace_back(VK_KHR_PORTABILITY_ENUMERATION_EXTENSION_NAME);

                createInfo.flags |= VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
            }
        }

        createInfo.enabledExtensionCount = static_cast<uint32_t>(enabledExtensions.size());
        createInfo.ppEnabledExtensionNames = enabledExtensions.data();

        // handle validation layers
        // and add additional debug messenger just for vkCreateInstance

        // placed outside if to make sure it is valid up to vkCreateInstance
        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
        if (m_EnabledValidationLayers)
        {
            createInfo.enabledLayerCount = static_cast<uint32_t>(m_ValidationLayers.size());
            createInfo.ppEnabledLayerNames = m_ValidationLayers.data();

            VulkanDebugMessenger::PopulateDebugMessengerCreateInfo(debugCreateInfo);
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

        const VkResult result = vkCreateInstance(&createInfo, nullptr, &m_VkInstance);
        SS_CORE_ASSERT(result == VK_SUCCESS, "Failed to create Vulkan instance!")

        // setup debug messenger after creating the m_Instance
        SetupDebugMessenger();
    }

    bool VulkanInstance::CheckValidationLayerSupport() const
    {
        // check if all of the requested layers are available
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

        for (const char* layerName : m_ValidationLayers)
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

    std::vector<const char*> VulkanInstance::GetRequiredExtensions() const
    {
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector requiredExtensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if (m_EnabledValidationLayers)
        {
            // add external debug message utilities - enables us to filter messages
            requiredExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        }

        return requiredExtensions;
    }

    void VulkanInstance::SetupDebugMessenger()
    {
        if (m_EnabledValidationLayers)
        {
            m_VulkanDebugMessenger = CreateScope<VulkanDebugMessenger>(m_VkInstance);
        }
    }
}
