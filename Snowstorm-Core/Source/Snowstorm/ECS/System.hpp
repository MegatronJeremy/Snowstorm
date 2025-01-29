#pragma once

#include <entt/entity/registry.hpp>

#include "Snowstorm/Core/Timestep.h"
#include "Snowstorm/Utility/NonCopyable.hpp"

namespace Snowstorm
{
	class System : public NonCopyable
	{
	public:
		using RegistryRef = entt::registry*;

		explicit System(const RegistryRef registry) : m_Registry(registry)
		{
		}

		virtual void execute(Timestep ts) = 0;

	protected:
		RegistryRef m_Registry;

		template <typename... Components>
		[[nodiscard]] auto view() const
		{
			return m_Registry->view<Components...>();
		}
	};
}
