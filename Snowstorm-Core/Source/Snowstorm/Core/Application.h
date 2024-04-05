#pragma once

#include "Window.h"
#include "Snowstorm/Core/LayerStack.h"
#include "Snowstorm/Events/Event.h"
#include "Snowstorm/Events/ApplicationEvent.h"

#include "Snowstorm/ImGui/ImGuiLayer.h"

namespace Snowstorm
{
	class Application
	{
	public:
		explicit Application(const std::string& name = "Snowstorm App");
		virtual ~Application();

		Application(const Application&) = delete;
		Application(Application&&) = delete;

		Application& operator=(const Application&) = delete;
		Application& operator=(Application&&) = delete;

		void Run();

		void OnEvent(Event& e);

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* layer);

		Window& GetWindow() const { return *m_Window; }

		void Close();

		ImGuiLayer* GetImGuiLayer() const { return m_ImGuiLayer; };

		static Application& Get() { return *s_Instance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(const WindowResizeEvent& e);

	private:
		Scope<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = true;
		bool m_Minimized = false;
		LayerStack m_LayerStack;
		float m_LastFrameTime = 0.0f;

	private:
		static Application* s_Instance;
	};

	// To be defined in CLIENT
	Application* CreateApplication();
}
