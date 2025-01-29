#pragma once

#include "Snowstorm/Core/Timestep.h"
#include "Snowstorm/Utility/NonCopyable.hpp"
#include "Snowstorm/ECS/SystemManager.hpp"

#include <entt/entt.hpp>

namespace Snowstorm
{
	class Entity;

	class Scene final : public NonCopyable
	{
	public:
		Scene();

		Entity createEntity(const std::string& name = std::string());

		void onUpdate(Timestep ts) const;
		void onViewportResize(uint32_t width, uint32_t height);

	private:
		entt::registry& getRegistry() { return m_SystemManager.getRegistry(); }

		SystemManager m_SystemManager;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		friend class Entity;
		friend class SceneHierarchyPanel;
	};
}
