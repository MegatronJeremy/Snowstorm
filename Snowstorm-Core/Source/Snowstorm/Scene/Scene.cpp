#include "pch.h"
#include "Scene.hpp"

#include "Components.h"

#include "Entity.h"
#include <Snowstorm/ECS/SystemManager.hpp>

#include "Snowstorm/Events/ApplicationEvent.h"
#include "Snowstorm/Events/Event.h"

#include "Snowstorm/Systems/CameraControllerSystem.hpp"
#include "Snowstorm/Systems/RenderSystem.hpp"
#include "Snowstorm/Systems/ScriptSystem.hpp"
#include "Snowstorm/Systems/ViewportResizeSystem.hpp"
#include "Snowstorm/Systems/WindowResizeEventSystem.hpp"

namespace Snowstorm
{
	Scene::Scene()
		: m_SystemManager(new SystemManager),
		  m_SingletonManager(new SingletonManager)
	{
		// TODO order of execution here is important
		m_SystemManager->registerSystem<ScriptSystem>(this);
		m_SystemManager->registerSystem<ViewportResizeSystem>(this);
		m_SystemManager->registerSystem<WindowResizeEventSystem>(this);
		m_SystemManager->registerSystem<CameraControllerSystem>(this);
		m_SystemManager->registerSystem<RenderSystem>(this);

		m_SingletonManager->registerSingleton<EventsHandlerSingleton>();
	}

	Scene::~Scene()
	{
		delete m_SystemManager;
		delete m_SingletonManager;
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
}
