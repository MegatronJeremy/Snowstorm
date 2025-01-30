#pragma once

#include <Snowstorm/ECS/System.hpp>

namespace Snowstorm
{
	class ScriptSystem final : public System
	{
	public:
		explicit ScriptSystem(const SceneRef context) : System(context)
		{
		}

		void execute(Timestep ts) override;
	};
}
