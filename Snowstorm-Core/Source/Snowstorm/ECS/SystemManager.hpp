#pragma once

#include <entt/entity/registry.hpp>

#include "System.hpp"
#include "Snowstorm/Core/Base.h"

namespace Snowstorm
{
	class System;

	class SystemManager final : public NonCopyable
	{
	public:
		template <typename T, typename... Args>
		void registerSystem(Args&&... args)
		{
			static_assert(std::is_base_of_v<System, T>, "T must inherit from BaseSystem");
			m_Systems.emplace_back(CreateScope<T>(&m_Registry, std::forward<Args>(args)...));
		}

		void executeSystems(const Timestep ts) const
		{
			for (auto& system : m_Systems)
			{
				system->execute(ts);
			}
		}

		entt::registry& getRegistry() { return m_Registry; }

	private:
		entt::registry m_Registry;
		std::vector<Scope<System>> m_Systems;
	};
}
