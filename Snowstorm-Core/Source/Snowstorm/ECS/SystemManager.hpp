#pragma once

#include "System.hpp"
#include "TrackedRegistry.hpp"

#include "Snowstorm/Core/Base.h"

namespace Snowstorm
{
	class SystemManager final : public NonCopyable
	{
	public:
		template <typename T, typename... Args>
		void RegisterSystem(Args&&... args)
		{
			static_assert(std::is_base_of_v<System, T>, "T must inherit from System");
			m_Systems.emplace_back(CreateScope<T>(std::forward<Args>(args)...));
		}

		void ExecuteSystems(const Timestep ts)
		{
			for (const auto& system : m_Systems)
			{
				system->Execute(ts);
			}

			m_Registry.clearTrackedComponents();
		}

		TrackedRegistry& GetRegistry() { return m_Registry; }

	private:
		TrackedRegistry m_Registry;
		std::vector<Scope<System>> m_Systems;
	};
}
