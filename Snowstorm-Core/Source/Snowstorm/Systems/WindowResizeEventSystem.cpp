#include "WindowResizeEventSystem.hpp"

#include "Snowstorm/Events/ApplicationEvent.h"
#include "Snowstorm/Events/Event.h"
#include "Snowstorm/Scene/Components.h"

namespace Snowstorm
{
	void WindowResizeEventSystem::execute(Timestep ts)
	{
		auto& eventsHandler = singletonView<EventsHandlerSingleton>();
		const auto cameraView = view<CameraComponent>();

		for (const auto& windowResizeEvent : eventsHandler.process<WindowResizeEvent>())
		{
			const uint32_t width = windowResizeEvent->m_Width;
			const uint32_t height = windowResizeEvent->m_Height;

			for (const auto entity : cameraView)
			{
				auto& camera = cameraView.get<CameraComponent>(entity);
				camera.Camera.setViewportSize(width, height);
			}
		}
	}
}
