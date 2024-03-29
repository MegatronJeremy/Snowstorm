#include "pch.h"
#include "Snowstorm/Core/Layer.h"

namespace Snowstorm
{
	Layer::Layer(std::string name)
		: m_DebugName(std::move(name))
	{
	}
}
