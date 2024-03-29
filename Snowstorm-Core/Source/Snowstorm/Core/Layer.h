#pragma once

#include "Snowstorm/Core/Timestep.h"
#include "Snowstorm/Events/Event.h"

namespace Snowstorm
{
	class Layer final
	{
	public:
		explicit Layer(std::string name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach()
		{
		}

		virtual void OnDetach()
		{
		}

		virtual void OnUpdate(Timestep ts)
		{
		}

		virtual void OnImGuiRender()
		{
		}

		virtual void OnEvent(Event& event)
		{
		}

		const std::string& GetName() const { return m_DebugName; }

	protected:
		std::string m_DebugName;
	};
}
