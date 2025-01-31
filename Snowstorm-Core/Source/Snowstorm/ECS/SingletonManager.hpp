#pragma once

#include <typeindex>

#include "Singleton.hpp"

#include "Snowstorm/Core/Base.h"
#include "Snowstorm/Core/Log.h"

namespace Snowstorm
{
	class SingletonManager
	{
	public:
		template <typename T, typename... Args>
		void registerSingleton(Args&&... args)
		{
			static_assert(std::is_base_of_v<Singleton, T>, "T must inherit from Singleton");

			if (const std::type_index typeIndex(typeid(T)); !m_Singletons.contains(typeIndex))
			{
				m_Singletons[typeIndex] = CreateScope<T>(std::forward<Args>(args)...);
			}
		}

		template <typename T>
		[[nodiscard]] T& getSingleton()
		{
			const std::type_index typeIndex(typeid(T));

			SS_ASSERT(m_Singletons.contains(typeIndex), "Singleton not registered!");

			return *static_cast<T*>(m_Singletons[typeIndex].get());
		}

	private:
		std::unordered_map<std::type_index, Scope<Singleton>> m_Singletons;
	};
}
