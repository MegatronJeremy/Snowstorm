#include "pch.h"

#include "RenderSystem.hpp"
#include <Snowstorm/Scene/Components.h>
#include <Snowstorm/Renderer/Renderer2D.h>

namespace Snowstorm
{
	void RenderSystem::execute(const Timestep ts)
	{
		const Camera* mainCamera = nullptr;
		const glm::mat4* cameraTransform = nullptr;

		for (const auto cameraView = view<TransformComponent, CameraComponent>(); const auto entity : cameraView)
		{
			auto [transform, camera] = cameraView.get(entity);
			if (camera.Primary)
			{
				mainCamera = &camera.Camera;
				cameraTransform = &transform.Transform;
				break;
			}
		}

		if (mainCamera)
		{
			Renderer2D::BeginScene(*mainCamera, *cameraTransform);

			for (const auto spriteView = view<TransformComponent, SpriteRendererComponent>(); const auto entity : spriteView)
			{
				auto [transform, sprite] = spriteView.get<TransformComponent, SpriteRendererComponent>(entity);
				Renderer2D::DrawQuad(transform, sprite.Color);
			}

			Renderer2D::EndScene();
		}
	}
}
