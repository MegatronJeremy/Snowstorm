#pragma once

#include "Snowstorm/Core/Base.h"
#include "Snowstorm/Scene/Scene.hpp"
#include "Snowstorm/Scene/Entity.h"

namespace Snowstorm
{
	class SceneHierarchyPanel
	{
	public:
		SceneHierarchyPanel() = default;
		explicit SceneHierarchyPanel(const Ref<Scene>& context);

		void setContext(const Ref<Scene>& context);

		void onImGuiRender();

	private:
		void drawEntityNode(Entity entity);

		static void drawComponents(Entity entity);

		Ref<Scene> m_Context = nullptr;
		Entity m_SelectionContext;
	};
}
