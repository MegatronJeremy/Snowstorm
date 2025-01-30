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
		void registerSystem(Args&&... args)
		{
			static_assert(std::is_base_of_v<System, T>, "T must inherit from BaseSystem");
			m_Systems.emplace_back(CreateScope<T>(std::forward<Args>(args)...));
		}

		void executeSystems(const Timestep ts)
		{
			for (const auto& system : m_Systems)
			{
				system->execute(ts);
			}

			m_Registry.clearTrackedComponents();
		}

		TrackedRegistry& getRegistry() { return m_Registry; }

	private:
		TrackedRegistry m_Registry;
		std::vector<Scope<System>> m_Systems;
	};
}
