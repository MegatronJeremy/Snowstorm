#include "RenderSystem.hpp"

#include <Snowstorm/Renderer/Renderer.hpp>

#include "Snowstorm/Events/ApplicationEvent.h"
#include "Snowstorm/Renderer/RenderCommand.h"
#include "Snowstorm/Scene/Components.h"

namespace Snowstorm
{
	void RenderSystem::execute(const Timestep ts)
	{
		const auto framebufferView = view<FramebufferComponent>();
		const auto cameraView = view<TransformComponent, CameraComponent, RenderTargetComponent>();
		const auto spriteView = view<TransformComponent, SpriteRendererComponent, RenderTargetComponent>();

		// Loop through each framebuffer
		for (const auto fbEntity : framebufferView)
		{
			const auto& framebufferComp = framebufferView.get<FramebufferComponent>(fbEntity);
			if (!framebufferComp.Active)
			{
				continue;
			}

			const Ref<Framebuffer> framebuffer = framebufferComp.Framebuffer;

			Renderer::ResetStats();
			framebuffer->Bind();
			RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
			RenderCommand::Clear();

			// Find the main camera that is linked to this framebuffer
			const Camera* mainCamera = nullptr;
			glm::mat4 cameraTransform{};

			for (const auto entity : cameraView)
			{
				if (const auto& [TargetFramebuffer] = cameraView.get<RenderTargetComponent>(entity); TargetFramebuffer == fbEntity) // Match framebuffer
				{
					auto [transform, camera] = cameraView.get<TransformComponent, CameraComponent>(entity);
					if (camera.Primary)
					{
						mainCamera = &camera.Camera;
						cameraTransform = transform;
						break;
					}
				}
			}

			if (!mainCamera)
			{
				framebuffer->Unbind();
				continue;
			}

			Renderer::BeginScene(*mainCamera, cameraTransform);

			for (const auto entity : spriteView)
			{
				if (auto& [TargetFramebuffer] = spriteView.get<RenderTargetComponent>(entity); TargetFramebuffer == fbEntity) // Match framebuffer
				{
					auto [transform, sprite] = spriteView.get<TransformComponent, SpriteRendererComponent>(entity);
					Renderer::DrawQuad(transform, sprite.Texture, sprite.TilingFactor, sprite.Color);
				}
			}

			Renderer::EndScene();
			framebuffer->Unbind();
		}
	}
}
