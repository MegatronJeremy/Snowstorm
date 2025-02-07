#pragma once

#include <Snowstorm.h>
#include "Panels/SceneHieararchyPanel.hpp"

namespace Snowstorm
{
	class EditorLayer final : public Layer
	{
	public:
		EditorLayer();
		~EditorLayer() override = default;

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate(Timestep ts) override;
		void OnImGuiRender() override;
		void OnEvent(Event& event) override;

	private:
		Ref<World> m_ActiveWorld;
		Entity m_FramebufferEntity;
		Entity m_SquareEntity;
		Entity m_CameraEntity;
		Entity m_SecondCamera;

		bool m_PrimaryCamera = true;

		// Panels
		SceneHierarchyPanel m_SceneHierarchyPanel;
	};
}
