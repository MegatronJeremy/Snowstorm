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

		void SetContext(const Ref<Scene>& context);

		void OnImGuiRender();

	private:
		void DrawEntityNode(Entity entity);

		static void DrawComponents(Entity entity);

		Ref<Scene> m_Context = nullptr;
		Entity m_SelectionContext;
	};
}
