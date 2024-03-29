#pragma once
#include <cstdint>
#include <optional>
#include <vector>

namespace Snowstorm
{
	// TODO move this to a separate file with other constants
#define MAX_FRAMES_IN_FLIGHT 2

	void PrintHelloWorld();

	void RenderingTest();

	struct QueueFamilyIndices
	{
		std::optional<uint32_t> graphicsFamily; // supports drawing commands
		std::optional<uint32_t> presentFamily; // supports presentation commands

		bool IsComplete() const
		{
			return graphicsFamily.has_value() && presentFamily.has_value();
		}
	};

	class HelloTriangleApplication
	{
	public:
		void Run();

	private:
		void InitWindow();

		static void PickPhysicalDevice();

		void CreateInstance() const;

		void InitVulkan() const;

		void SetupDebugMessenger() const;

		static void CreateSurface();

		void CreateLogicalDevice() const;

		static void CreateSwapChain();

		static void CreateImageViews();

		static void CreateRenderPass();

		static void CreateGraphicsPipeline();

		static void CreateFramebuffers();

		static void CreateCommandPool();

		static void CreateVertexBuffer();

		static void CreateCommandBuffers();

		static void CreateSyncObjects();

		static void MainLoop();

		static void DrawFrame();

		static void CleanupSwapChain();

		static void RecreateSwapChain();

		void Cleanup() const;

		bool CheckValidationLayerSupport() const;

		std::vector<const char*> GetRequiredExtensions() const;

	private:
		uint32_t width = 800;
		uint32_t height = 600;

		std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
	};
}
