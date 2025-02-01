#include "pch.h"
#include "Application.h"

#include <GLFW/glfw3.h>

#include <ranges>

#include "Snowstorm/Renderer/Renderer.hpp"

namespace Snowstorm
{
#define BIND_EVENT_FN(x) [this](auto&&... args) { return this->x(std::forward<decltype(args)>(args)...); }

	Application* Application::s_Instance = nullptr;

	Application::Application(const std::string& name)
	{
		SS_PROFILE_FUNCTION();

		SS_CORE_ASSERT(!s_Instance, "Application already exists!")
		s_Instance = this;
		m_Window = Window::Create(WindowProps(name));
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Renderer::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
		SS_PROFILE_FUNCTION();

		Renderer::Shutdown();
	}

	void Application::Run()
	{
		SS_PROFILE_FUNCTION();

		while (m_Running)
		{
			SS_PROFILE_SCOPE("RunLoop");

			const auto time = static_cast<float>(glfwGetTime()); // Platform::GetTime
			const Timestep timestep = time - m_LastFrameTime;
			m_LastFrameTime = time;

			// Pause when minimized
			if (!m_Minimized)
			{
				SS_PROFILE_SCOPE("LayerStack OnUpdate");

				for (Layer* layer : m_LayerStack)
				{
					layer->OnUpdate(timestep);
				}
			}

			m_ImGuiLayer->Begin();
			{
				SS_PROFILE_SCOPE("LayerStack OnImGuiRender");

				for (Layer* layer : m_LayerStack)
				{
					layer->OnImGuiRender();
				}
			}
			m_ImGuiLayer->End();

			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		SS_PROFILE_FUNCTION();

		EventDispatcher dispatcher(e);
		dispatcher.dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));
		dispatcher.dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));

		if (e.Handled)
		{
			return;
		}

		for (const auto& it : std::ranges::reverse_view(m_LayerStack))
		{
			it->OnEvent(e);
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		SS_PROFILE_FUNCTION();

		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* layer)
	{
		SS_PROFILE_FUNCTION();

		m_LayerStack.PushOverlay(layer);
		layer->OnAttach();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		SS_PROFILE_FUNCTION();

		e.Handled = true;
		if (e.Width == 0 || e.Height == 0)
		{
			m_Minimized = true;
			return true;
		}

		m_Minimized = false;
		return true;
	}
}
