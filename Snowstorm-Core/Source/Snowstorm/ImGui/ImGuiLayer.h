#pragma once

#include "Snowstorm/Core/Layer.h"

namespace Snowstorm
{
	class ImGuiLayer final : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() override;

		ImGuiLayer(const ImGuiLayer&) = delete;
		ImGuiLayer(ImGuiLayer&&) = delete;

		ImGuiLayer& operator=(const ImGuiLayer&) = delete;
		ImGuiLayer& operator=(ImGuiLayer&&) = delete;

		void OnAttach() override;
		void OnDetach() override;

		void OnEvent(Event& e) override;

		void Begin();
		void End();

		void BlockEvents(const bool block) { m_BlockEvents = block; }

	private:
		bool m_BlockEvents = true;
	};
}
