#pragma once

#include "Snowstorm/Core/Base.h"
#include "Snowstorm/World/World.hpp"
#include "Snowstorm/World/Entity.h"

namespace Snowstorm
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		explicit SceneHierarchyPanel(const Ref<World>& context);

		void setContext(const Ref<World>& context);

		void onImGuiRender();

	private:
		void drawEntityNode(Entity entity);

		static void drawComponents(Entity entity);

		Ref<World> m_Context = nullptr;
		Entity m_SelectionContext;
	};
}
