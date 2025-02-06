#pragma once

#include "Snowstorm/Core/Timestep.h"
#include "Snowstorm/ECS/SingletonManager.hpp"
#include "Snowstorm/ECS/TrackedRegistry.hpp"
#include "Snowstorm/Service/ServiceManager.hpp"
#include "Snowstorm/Utility/NonCopyable.hpp"

namespace Snowstorm
{
	class SystemManager;
	class Entity;

	class World final : public NonCopyable
	{
	public:
		World();
		~World() override;

		Entity CreateEntity(const std::string& name = std::string());

		[[nodiscard]] TrackedRegistry& GetRegistry() const;

		template <typename T>
		T& GetSingleton() const
		{
			return m_SingletonManager->GetSingleton<T>();
		}

		void OnUpdate(Timestep ts) const;
		void PostUpdate(Timestep ts) const;

	private:
		ServiceManager* m_ServiceManager;
		SystemManager* m_SystemManager;
		SingletonManager* m_SingletonManager;

		friend class Entity;
		friend class SceneHierarchyPanel;
	};
}
