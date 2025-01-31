#pragma once
#include "Snowstorm/ECS/System.hpp"

namespace Snowstorm
{
	class CameraControllerSystem final : public System
	{
	public:
		explicit CameraControllerSystem(const SceneRef scene) : System(scene)
		{
		}

		void execute(Timestep ts) override;
	};
}
