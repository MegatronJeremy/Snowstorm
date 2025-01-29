#pragma once

#include "Scene.hpp"
#include <Snowstorm/Core/Log.h>

#include <entt/entt.hpp>

namespace Snowstorm
{
	class Entity
	{
	public:
		Entity() = default;
		~Entity() = default;

		Entity(entt::entity handle, Scene* scene);

		Entity(const Entity&) = default;
		Entity(Entity&&) = default;

		Entity& operator=(const Entity&) = default;
		Entity& operator=(Entity&&) = default;

		template <typename T, typename... Args>
		T& addComponent(Args&&... args)
		{
			SS_CORE_ASSERT(!hasComponent<T>(), "Entity already has component!");
			return m_Scene->getRegistry().emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
		}

		template <typename T>
		T& getComponent()
		{
			SS_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
			return m_Scene->getRegistry().get<T>(m_EntityHandle);
		}

		template <typename T>
		[[nodiscard]] bool hasComponent() const
		{
			return m_Scene->getRegistry().any_of<T>(m_EntityHandle);
		}

		template <typename T>
		void removeComponent() const
		{
			SS_CORE_ASSERT(hasComponent<T>(), "Entity does not have component!");
			m_Scene->getRegistry().remove<T>(m_EntityHandle);
		}

		operator bool() const { return m_EntityHandle != entt::null; }
		operator uint32_t() const { return static_cast<uint32_t>(m_EntityHandle); }

		bool operator==(const Entity& other) const
		{
			return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}

	private:
		entt::entity m_EntityHandle = entt::null;
		Scene* m_Scene = nullptr;
	};
}
