#include "pch.h"
#include "Scene.hpp"

#include "Components.h"

#include "Entity.h"
#include <Snowstorm/Scene/Systems/RenderSystem.hpp>
#include <Snowstorm/Scene/Systems/ScriptSystem.hpp>
#include <Snowstorm/ECS/SystemManager.hpp>

namespace Snowstorm
{
	Scene::Scene()
		: m_SystemManager(new SystemManager)
	{
		m_SystemManager->registerSystem<RenderSystem>(this);
		m_SystemManager->registerSystem<ScriptSystem>(this);
	}

	Scene::~Scene()
	{
		delete m_SystemManager;
	}

	Entity Scene::createEntity(const std::string& name)
	{
		Entity entity = {m_SystemManager->getRegistry().create(), this};
		entity.addComponent<TransformComponent>();
		auto& tag = entity.addComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	TrackedRegistry& Scene::getRegistry() const
	{
		return m_SystemManager->getRegistry();
	}

	void Scene::onUpdate(const Timestep ts) const
	{
		m_SystemManager->executeSystems(ts);
	}

	void Scene::onViewportResize(const uint32_t width, const uint32_t height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;

		// Resize all non-fixed aspect ratio cameras
		// TODO create an event system for this
		for (const auto view = m_SystemManager->getRegistry().m_Registry.view<CameraComponent>(); const auto entity : view)
		{
			if (auto& cameraComponent = view.get<CameraComponent>(entity); !cameraComponent.FixedAspectRatio)
			{
				cameraComponent.Camera.setViewportSize(width, height);
			}
		}
	}
}
