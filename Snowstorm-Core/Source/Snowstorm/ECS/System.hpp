#pragma once

#include <entt/entt.hpp>

#include <Snowstorm/Core/Timestep.h>
#include <Snowstorm/Utility/NonCopyable.hpp>
#include "Snowstorm/Scene/Scene.hpp"

namespace Snowstorm
{
	class System : public NonCopyable
	{
	public:
		using SceneRef = Scene*;

		explicit System(const SceneRef context)
			: m_Scene(context)
		{
		}

		/// Function that derived systems override
		virtual void Execute(Timestep ts) = 0;

	protected:
		/// Standard entity view for active components
		template <typename... Components>
		[[nodiscard]] auto View() const
		{
			static_assert(sizeof...(Components) > 0, "view requires at least one component type.");

			// TODO make this look better
			return m_Scene->GetRegistry().m_Registry.view<Components...>();
		}

		/// Returns a view of entities that had all the specified components added
		template <typename... Components>
		[[nodiscard]] auto InitView() const
		{
			static_assert(sizeof...(Components) > 0, "initView requires at least one component type.");

			std::unordered_set<entt::entity> entitiesWithAddedComponents;

			for (const auto& [entity, componentTypes] : m_Scene->GetRegistry().m_AddedComponents)
			{
				if ((componentTypes.count(std::type_index(typeid(Components))) && ...))
				{
					entitiesWithAddedComponents.insert(entity);
				}
			}

			return entitiesWithAddedComponents;
		}

		/// Returns a view of entities that had all the specified components removed
		template <typename... Components>
		[[nodiscard]] auto FiniView() const
		{
			static_assert(sizeof...(Components) > 0, "finiView requires at least one component type.");

			std::unordered_set<entt::entity> entitiesWithRemovedComponents;

			for (const auto& [entity, componentTypes] : m_Scene->GetRegistry().m_RemovedComponents)
			{
				if ((componentTypes.count(std::type_index(typeid(Components))) && ...))
				{
					entitiesWithRemovedComponents.insert(entity);
				}
			}

			return entitiesWithRemovedComponents;
		}

		/// Returns a singleton present in the system's context
		template <typename T>
		[[nodiscard]] T& SingletonView()
		{
			return m_Scene->getSingletonManager().GetSingleton<T>();
		}

		SceneRef m_Scene;
	};
}
