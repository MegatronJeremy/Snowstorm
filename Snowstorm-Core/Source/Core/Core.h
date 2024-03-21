#pragma once
#include <cstdint>

namespace Core
{

	void PrintHelloWorld();

	void RenderingTest();

	class HelloTriangleApplication
	{
	public:
		void Run() const;

	private:
		void InitWindow() const;

		static void CreateInstance();

		static void InitVulkan();

		static void MainLoop();

		static void Cleanup();

	private:
		const uint32_t width = 800;
		const uint32_t height = 600;
	};


}
