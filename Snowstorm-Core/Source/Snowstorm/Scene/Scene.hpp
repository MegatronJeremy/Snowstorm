#pragma once

#include "Snowstorm/Core/Timestep.h"
#include "Snowstorm/ECS/SingletonManager.hpp"
#include "Snowstorm/Utility/NonCopyable.hpp"
#include "Snowstorm/ECS/TrackedRegistry.hpp"

namespace Snowstorm
{
	class SystemManager;
	class Entity;

	class Scene final : public NonCopyable
	{
	public:
		Scene();
		~Scene() override;

		Entity CreateEntity(const std::string& name = std::string());

		[[nodiscard]] TrackedRegistry& GetRegistry() const;
		[[nodiscard]] SingletonManager& getSingletonManager() const { return *m_SingletonManager; }

		void OnUpdate(Timestep ts) const;

	private:
		SystemManager* m_SystemManager;
		SingletonManager* m_SingletonManager;

		friend class Entity;
		friend class SceneHierarchyPanel;
	};
}
