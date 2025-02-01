#pragma once
#include "Snowstorm/ECS/System.hpp"

namespace Snowstorm
{
	class ViewportResizeSystem final : public System
	{
	public:
		explicit ViewportResizeSystem(const SceneRef scene)
			: System(scene)
		{
		}

		void Execute(Timestep ts) override;
	};
}
