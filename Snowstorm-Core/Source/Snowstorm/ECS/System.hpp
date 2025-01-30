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
			: m_context(context)
		{
		}

		/// Function that derived systems override
		virtual void execute(Timestep ts) = 0;

	protected:
		/// Standard entity view for active components
		template <typename... Components>
		[[nodiscard]] auto view() const
		{
			static_assert(sizeof...(Components) > 0, "view requires at least one component type.");

			// TODO make this look better
			return m_context->getRegistry().m_Registry.view<Components...>();
		}

		/// Returns a view of entities that had all the specified components added
		template <typename... Components>
		[[nodiscard]] auto initView() const
		{
			static_assert(sizeof...(Components) > 0, "initView requires at least one component type.");

			std::unordered_set<entt::entity> entitiesWithAddedComponents;

			for (const auto& [entity, componentTypes] : m_context->getRegistry().m_AddedComponents)
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
		[[nodiscard]] auto finiView() const
		{
			static_assert(sizeof...(Components) > 0, "finiView requires at least one component type.");

			std::unordered_set<entt::entity> entitiesWithRemovedComponents;

			for (const auto& [entity, componentTypes] : m_context->getRegistry().m_RemovedComponents)
			{
				if ((componentTypes.count(std::type_index(typeid(Components))) && ...))
				{
					entitiesWithRemovedComponents.insert(entity);
				}
			}

			return entitiesWithRemovedComponents;
		}

		SceneRef m_context;
	};
}
