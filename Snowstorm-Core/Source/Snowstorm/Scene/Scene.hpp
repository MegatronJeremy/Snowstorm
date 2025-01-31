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

		Entity createEntity(const std::string& name = std::string());

		[[nodiscard]] TrackedRegistry& getRegistry() const;
		[[nodiscard]] SingletonManager& getSingletonManager() const { return *m_SingletonManager; }

		void onUpdate(Timestep ts) const;
		void onViewportResize(uint32_t width, uint32_t height);

	private:
		SystemManager* m_SystemManager;
		SingletonManager* m_SingletonManager;

		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		friend class Entity;
		friend class SceneHierarchyPanel;
	};
}
