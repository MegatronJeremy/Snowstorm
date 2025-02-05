#pragma once
#include "Snowstorm/ECS/System.hpp"

namespace Snowstorm
{
	class MandelbrotControllerSystem final : public System
	{
	public:
		explicit MandelbrotControllerSystem(const SceneRef scene) : System(scene)
		{
		}

		void Execute(Timestep ts) override;
	};
}
