#include "pch.h"
#include "World.hpp"

#include "Components.hpp"

#include "Entity.h"
#include <Snowstorm/ECS/SystemManager.hpp>

#include "Snowstorm/Events/Event.h"
#include "Snowstorm/Render/MeshLibrarySingleton.hpp"
#include "Snowstorm/Render/Renderer3DSingleton.hpp"
#include "Snowstorm/Render/Shader.hpp"
#include "Snowstorm/Service/ImGuiService.hpp"

#include "Snowstorm/System/CameraControllerSystem.hpp"
#include "Snowstorm/System/RenderSystem.hpp"
#include "Snowstorm/System/ScriptSystem.hpp"
#include "Snowstorm/System/ShaderReloadSystem.hpp"
#include "Snowstorm/System/ViewportResizeSystem.hpp"

namespace Snowstorm
{
	World::World()
		: m_ServiceManager(new ServiceManager),
		  m_SystemManager(new SystemManager),
		  m_SingletonManager(new SingletonManager)
	{
		// TODO also, don't hardcode this. This should be modifiable for all worlds and read from the world settings

		// TODO important that this service executes before all other ImGui editor services/systems
		m_ServiceManager->RegisterService<ImGuiService>();

		// TODO order of execution here is important, create some sort of execution graph
		m_SystemManager->RegisterSystem<ScriptSystem>(this);
		m_SystemManager->RegisterSystem<ViewportResizeSystem>(this);
		m_SystemManager->RegisterSystem<CameraControllerSystem>(this);
		m_SystemManager->RegisterSystem<ShaderReloadSystem>(this);
		m_SystemManager->RegisterSystem<RenderSystem>(this);

		m_SingletonManager->RegisterSingleton<EventsHandlerSingleton>();
		m_SingletonManager->RegisterSingleton<ShaderLibrarySingleton>();
		m_SingletonManager->RegisterSingleton<MeshLibrarySingleton>();
		m_SingletonManager->RegisterSingleton<Renderer3DSingleton>();
	}

	World::~World()
	{
		delete m_SystemManager;
		delete m_SingletonManager;
		delete m_ServiceManager;
	}

	Entity World::CreateEntity(const std::string& name)
	{
		Entity entity = {m_SystemManager->GetRegistry().create(), this};

		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;

		return entity;
	}

	TrackedRegistry& World::GetRegistry() const
	{
		return m_SystemManager->GetRegistry();
	}

	void World::OnUpdate(const Timestep ts) const
	{
		m_ServiceManager->ExecuteUpdate(ts);
		m_SystemManager->ExecuteSystems(ts);
	}
	void World::PostUpdate(const Timestep ts) const
	{
		m_ServiceManager->ExecutePostUpdate(ts);
	}
}
