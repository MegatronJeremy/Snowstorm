#pragma once

#include "Snowstorm/ECS/System.hpp"

namespace Snowstorm
{
	class RenderSystem final : public System
	{
	public:
		explicit RenderSystem(const SceneRef scene) : System(scene)
		{
		}

		void Execute(Timestep ts) override;
	};
}
