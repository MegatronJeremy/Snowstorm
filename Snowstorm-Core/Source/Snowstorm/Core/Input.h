#pragma once

#include "Snowstorm/Core/KeyCodes.h"
#include "Snowstorm/Core/MouseCodes.h"

namespace Snowstorm
{
	class Input
	{
	public:
		static bool IsKeyPressed(KeyCode key);

		static bool IsMouseButtonPressed(MouseCode button);
		static std::pair<float, float> GetMousePosition();
		static float GetMouseX();
		static float GetMouseY();
	};
}
