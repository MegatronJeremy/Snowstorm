#pragma once

namespace Snowstorm {

	namespace RendererUtils {

		struct ResourceAllocationCounts
		{
			uint32_t Samplers = 0;
		};

		ResourceAllocationCounts& GetResourceAllocationCounts();
	}
}
