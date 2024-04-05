#pragma once

#undef INFINITE
#include "msdf-atlas-gen.h"

#include <vector>

namespace Snowstorm {

	struct MSDFData
	{
		msdf_atlas::FontGeometry FontGeometry;
		std::vector<msdf_atlas::GlyphGeometry> Glyphs;
	};

}

