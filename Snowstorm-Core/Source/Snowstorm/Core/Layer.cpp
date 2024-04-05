#include "pch.h"
#include "Layer.h"

namespace Snowstorm
{
	Layer::Layer(std::string debugName)
		: m_DebugName(std::move(debugName))
	{
	}

	Layer::~Layer()
	= default;
}
