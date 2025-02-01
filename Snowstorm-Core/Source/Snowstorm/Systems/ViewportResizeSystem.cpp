#include "ViewportResizeSystem.hpp"

#include "Snowstorm/Events/ApplicationEvent.h"
#include "Snowstorm/Events/Event.h"
#include "Snowstorm/Scene/Components.h"

namespace Snowstorm
{
	void ViewportResizeSystem::execute(Timestep ts)
	{
		for (const auto viewportView = view<ViewportComponent, FramebufferComponent>(); const auto entity : viewportView)
		{
			const auto& [Size, Focused, Hovered] = viewportView.get<ViewportComponent>(entity);
			const auto& framebuffer = viewportView.get<FramebufferComponent>(entity).Framebuffer;

			if (Size.x <= 0.0f || Size.y <= 0.0f)
			{
				continue;
			}

			if (const auto [Width, Height, Samples, SwapChainTarget] = framebuffer->GetSpecification();
				Width != static_cast<uint32_t>(Size.x) || Height != static_cast<uint32_t>(Size.y))
			{
				framebuffer->Resize(static_cast<uint32_t>(Size.x), static_cast<uint32_t>(Size.y));

				auto& eventsHandler = singletonView<EventsHandlerSingleton>();
				eventsHandler.pushEvent<WindowResizeEvent>(Size.x, Size.y);
			}
		}
	}
}
