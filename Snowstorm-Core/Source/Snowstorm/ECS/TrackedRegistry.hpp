#pragma once

#include <entt/entt.hpp>
#include <unordered_set>
#include <unordered_map>
#include <typeindex>

namespace Snowstorm
{
	class TrackedRegistry
	{
	public:
		TrackedRegistry() = default;

		/// Overload `create()` to track entity creation (optional)
		entt::entity create()
		{
			return m_Registry.create();
		}

		/// Overload `destroy()` to track when an entity is destroyed
		void destroy(const entt::entity entity)
		{
			m_DestroyedEntities.insert(entity);
			m_AddedComponents.erase(entity);
			m_RemovedComponents.erase(entity);
			m_AccessedComponents.erase(entity);

			m_Registry.destroy(entity);
		}

		/// Overload `emplace<T>()` to track when a component is added
		template <typename T, typename... Args>
		T& emplace(entt::entity entity, Args&&... args)
		{
			const std::type_index typeIndex(typeid(T));

			// If component was removed in the same frame, remove from removed list
			m_RemovedComponents[entity].erase(typeIndex);
			m_AddedComponents[entity].insert(typeIndex);

			return m_Registry.emplace<T>(entity, std::forward<Args>(args)...);
		}

		/// Overload `remove<T>()` to track when a component is removed
		template <typename T>
		void remove(const entt::entity entity)
		{
			const std::type_index typeIndex(typeid(T));

			// If component was added in the same frame, undo the addition
			m_AddedComponents[entity].erase(typeIndex);
			m_RemovedComponents[entity].insert(typeIndex);

			m_Registry.remove<T>(entity);
		}

		/// Overload `get<T>()` to track when a component is accessed
		template <typename T>
		T& get(const entt::entity entity)
		{
			const std::type_index typeIndex(typeid(T));

			// Track that this component was accessed
			m_AccessedComponents[entity].insert(typeIndex);

			return m_Registry.get<T>(entity);
		}

		/// Overload `any_of<T>()`
		template <typename T>
		[[nodiscard]] bool any_of(const entt::entity entity) const
		{
			return m_Registry.any_of<T>(entity);
		}

		/// Clears tracked component events (call this per frame after processing)
		void clearTrackedComponents()
		{
			m_AddedComponents.clear();
			m_RemovedComponents.clear();
			m_AccessedComponents.clear();
			m_DestroyedEntities.clear();
		}

		/// TODO move this to private later
		entt::registry m_Registry;

		/// Track component lifecycle events
		std::unordered_map<entt::entity, std::unordered_set<std::type_index>> m_AddedComponents;
		std::unordered_map<entt::entity, std::unordered_set<std::type_index>> m_RemovedComponents;
		std::unordered_map<entt::entity, std::unordered_set<std::type_index>> m_AccessedComponents;
		std::unordered_set<entt::entity> m_DestroyedEntities;
	};
}
