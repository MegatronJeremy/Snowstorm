#include "ViewportResizeSystem.hpp"

#include "Snowstorm/Events/ApplicationEvent.h"
#include "Snowstorm/Scene/Components.hpp"

namespace Snowstorm
{
	void ViewportResizeSystem::Execute(Timestep ts)
	{
		const auto viewportView = View<ViewportComponent, FramebufferComponent>();
		const auto cameraView = View<CameraComponent, RenderTargetComponent>();

		for (const auto entity : viewportView)
		{
			const auto [viewport, framebuffer] = viewportView.get<ViewportComponent, FramebufferComponent>(entity);

			if (viewport.Size.x <= 0.0f || viewport.Size.y <= 0.0f)
			{
				continue;
			}

			const uint32_t viewportWidth = static_cast<uint32_t>(viewport.Size.x);
			const uint32_t viewportHeight = static_cast<uint32_t>(viewport.Size.y);

			if (const auto fbSpec = framebuffer.Framebuffer->GetSpecification(); fbSpec.Width == viewportWidth && fbSpec
				.Width == viewportHeight)
			{
				continue;
			}

			framebuffer.Framebuffer->Resize(viewportWidth, viewportHeight);

			// Resize all camera viewports within the framebuffer
			for (const auto& cameraEntity : cameraView)
			{
				if (const auto [camera, renderTarget] = cameraView.get<
					CameraComponent, RenderTargetComponent>(cameraEntity); renderTarget.TargetFramebuffer == entity)
				{
					camera.Camera.SetViewportSize(viewportWidth, viewportHeight);
				}
			}
		}
	}
}
