#pragma once

#include "Snowstorm/ECS/System.hpp"

namespace Snowstorm
{
	class RenderSystem final : public System
	{
	public:
		explicit RenderSystem(const SceneRef context) : System(context)
		{
		}

		void execute(Timestep ts) override;
	};
}
