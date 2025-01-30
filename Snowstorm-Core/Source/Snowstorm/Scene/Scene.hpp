#pragma once

#include "Snowstorm/Core/Timestep.h"
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

		void onUpdate(Timestep ts) const;
		void onViewportResize(uint32_t width, uint32_t height);

	private:
		SystemManager* m_SystemManager;
		uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		friend class Entity;
		friend class SceneHierarchyPanel;
	};
}
