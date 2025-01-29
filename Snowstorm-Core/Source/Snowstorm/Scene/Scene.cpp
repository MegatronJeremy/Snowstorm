#include "pch.h"
#include "Scene.hpp"

#include "Components.h"

#include "Entity.h"
#include <Snowstorm/Scene/Systems/RenderSystem.hpp>

namespace Snowstorm
{
	Scene::Scene()
	{
		m_SystemManager.registerSystem<RenderSystem>();
	}

	Entity Scene::createEntity(const std::string& name)
	{
		Entity entity = {m_SystemManager.getRegistry().create(), this};
		entity.addComponent<TransformComponent>();
		auto& tag = entity.addComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::onUpdate(const Timestep ts) const
	{
		m_SystemManager.executeSystems(ts);
	}

	void Scene::onViewportResize(const uint32_t width, const uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize all non-fixed aspect ratio cameras
		// TODO move this to a system?
		for (const auto view = m_SystemManager.getRegistry().view<CameraComponent>(); const auto entity : view)
		{
			if (auto& cameraComponent = view.get<CameraComponent>(entity); !cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.SetViewportSize(width, height);
			}
		}
	}
}
