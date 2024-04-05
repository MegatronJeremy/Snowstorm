#pragma once

#include "Snowstorm/Core/Ref.h"

struct GLFWwindow;

namespace Snowstorm {

	class RendererContext : public RefCounted
	{
	public:
		RendererContext() = default;
		virtual ~RendererContext() = default;

		virtual void Init() = 0;

		static Ref<RendererContext> Create();
	};

}
