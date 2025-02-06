#pragma once

#include "Snowstorm/Core/Base.h"
#include "Snowstorm/Core/Timestep.h"
#include "Snowstorm/Events/Event.h"

namespace Snowstorm
{
	class Layer
	{
	public:
		explicit Layer(std::string debugName = "Layer");
		virtual ~Layer();

		Layer(const Layer&) = delete;
		Layer(Layer&&) = delete;

		Layer& operator=(const Layer&) = delete;
		Layer& operator=(Layer&&) = delete;

		virtual void OnAttach()
		{
		}

		virtual void OnDetach()
		{
		}

		virtual void OnUpdate(Timestep ts)
		{
		}

		virtual void PostUpdate(Timestep ts)
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
