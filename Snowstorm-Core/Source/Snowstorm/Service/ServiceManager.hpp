#pragma once
#include <vector>

#include "Service.hpp"

#include "Snowstorm/Core/Base.h"

namespace Snowstorm
{
	class ServiceManager
	{
	public:
		template <typename T, typename... Args>
		void RegisterService(Args&&... args)
		{
			static_assert(std::is_base_of_v<Service, T>, "T must inherit from System");
			m_Services.emplace_back(CreateScope<T>(std::forward<Args>(args)...));
		}

		void ExecuteUpdate(const Timestep ts) const
		{
			for (const auto& service : m_Services)
			{
				service->OnUpdate(ts);
			}
		}

		void ExecutePostUpdate(const Timestep ts) const
		{
			for (const auto& service : m_Services)
			{
				service->PostUpdate(ts);
			}
		}

	private:
		std::vector<Scope<Service>> m_Services;
	};
}
