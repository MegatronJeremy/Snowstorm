#pragma once
#include <cstdint>
#include <optional>
#include <vector>

namespace Core
{
	void PrintHelloWorld();

	void RenderingTest();

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily;

		bool IsComplete() const
		{
			return graphicsFamily.has_value();
		}
	};

	class HelloTriangleApplication
	{
	public:
		void Run() const;

	private:
		void InitWindow() const;

		static void PickPhysicalDevice();

		void CreateInstance() const;

		void InitVulkan() const;

		void SetupDebugMessenger() const;

		static void MainLoop();

		void Cleanup() const;

		bool CheckValidationLayerSupport() const;

		std::vector<const char*> GetRequiredExtensions() const;

	private:
		uint32_t width = 800;
		uint32_t height = 600;

		std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};

#ifdef DEBUG
		bool enableValidationLayers = true;
#else
		bool enableValidationLayers = false;
#endif
	};
}
