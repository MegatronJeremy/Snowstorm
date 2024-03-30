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


	class HelloTriangleApplication
	{
	public:
		void Run();

	private:
		void InitWindow();

		static void PickPhysicalDevice();

		void CreateInstance() const;

		void InitVulkan() const;

		static void SetupDebugMessenger();

		static void CreateSurface();

		void CreateLogicalDevice() const;

		static void CreateSwapChain();

		static void CreateImageViews();

		static void CreateRenderPass();

		static void CreateGraphicsPipeline();

		static void CreateFramebuffers();

		static void CreateCommandPool();

		static void CreateVertexBuffer();

		static void CreateIndexBuffer();

		static void CreateCommandBuffers();

		static void CreateSyncObjects();

		static void MainLoop();

		static void DrawFrame();

		static void CleanupSwapChain();

		static void RecreateSwapChain();

		static void Cleanup();

		bool CheckValidationLayerSupport() const;

		static std::vector<const char*> GetRequiredExtensions();

	private:
		uint32_t width = 800;
		uint32_t height = 600;

		std::vector<const char*> validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
	};
}
