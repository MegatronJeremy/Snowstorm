#pragma once

#include "Snowstorm/Asset/Asset.h"

namespace Snowstorm {

	using ResourceDescriptorInfo = void*;

	class RendererResource : public Asset
	{
	public:
		virtual ResourceDescriptorInfo GetDescriptorInfo() const = 0;
	};

}
