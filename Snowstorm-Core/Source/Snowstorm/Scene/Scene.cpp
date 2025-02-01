#include "pch.h"
#include "Scene.hpp"

#include "Components.h"

#include "Entity.h"
#include <Snowstorm/ECS/SystemManager.hpp>

#include "Snowstorm/Events/Event.h"
#include "Snowstorm/Renderer/Shader.hpp"

#include "Snowstorm/Systems/CameraControllerSystem.hpp"
#include "Snowstorm/Systems/RenderSystem.hpp"
#include "Snowstorm/Systems/ScriptSystem.hpp"
#include "Snowstorm/Systems/ShaderReloadSystem.hpp"
#include "Snowstorm/Systems/ViewportResizeSystem.hpp"

namespace Snowstorm
{
	Scene::Scene()
		: m_SystemManager(new SystemManager),
		  m_SingletonManager(new SingletonManager)
	{
		// TODO order of execution here is important
		m_SystemManager->registerSystem<ScriptSystem>(this);
		m_SystemManager->registerSystem<ViewportResizeSystem>(this);
		m_SystemManager->registerSystem<CameraControllerSystem>(this);
		m_SystemManager->registerSystem<ShaderReloadSystem>(this);
		m_SystemManager->registerSystem<RenderSystem>(this);

		m_SingletonManager->RegisterSingleton<EventsHandlerSingleton>();
		m_SingletonManager->RegisterSingleton<ShaderLibrarySingleton>();
	}

	Scene::~Scene()
	{
		delete m_SystemManager;
		delete m_SingletonManager;
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = {m_SystemManager->getRegistry().create(), this};

		auto& tag = entity.addComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	TrackedRegistry& Scene::GetRegistry() const
	{
		return m_SystemManager->getRegistry();
	}

	void Scene::OnUpdate(const Timestep ts) const
	{
		m_SystemManager->executeSystems(ts);
	}
}
