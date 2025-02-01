#pragma once

#include "Snowstorm/ECS/System.hpp"

namespace Snowstorm
{
	class WindowResizeEventSystem : public System
	{
	public:
		explicit WindowResizeEventSystem(const SceneRef context)
			: System(context)
		{
		}

		void execute(Timestep ts) override;
	};
}
