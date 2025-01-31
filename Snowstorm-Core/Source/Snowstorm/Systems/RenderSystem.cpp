#include "RenderSystem.hpp"

#include <Snowstorm/Renderer/Renderer2D.h>

#include "Snowstorm/Events/ApplicationEvent.h"
#include "Snowstorm/Events/Event.h"
#include "Snowstorm/Scene/Components.h"

namespace Snowstorm
{
	void RenderSystem::execute(const Timestep ts)
	{
		const auto cameraView = view<TransformComponent, CameraComponent>();
		const auto spriteView = view<TransformComponent, SpriteRendererComponent>();
		auto& eventsHandler = singletonView<EventsHandlerSingleton>();

		const Camera* mainCamera = nullptr;
		glm::mat4 cameraTransform{};

		for (const auto entity : cameraView)
		{
			auto [transform, camera] = cameraView.get(entity);
			if (camera.Primary)
			{
				mainCamera = &camera.Camera;
				cameraTransform = transform;
				break;
			}
		}

		for (const auto& windowResizeEvent : eventsHandler.process<WindowResizeEvent>())
		{
			const uint32_t width = windowResizeEvent->m_Width;
			const uint32_t height = windowResizeEvent->m_Height;

			for (const auto entity : cameraView)
			{
				auto [_, camera] = cameraView.get(entity);
				camera.Camera.setViewportSize(width, height);
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, cameraTransform);

			for (const auto entity : spriteView)
			{
				auto [transform, sprite] = spriteView.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawQuad(transform, sprite.Color);
			}

			Renderer2D::EndScene();
		}
	}
}
