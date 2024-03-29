#pragma once

#include "Snowstorm/Core/PlatformDetection.h"

#include <memory>

#ifdef SS_DEBUG
#if defined(SS_PLATFORM_WINDOWS)
#define SS_DEBUGBREAK() __debugbreak()
#elif defined(SS_PLATFORM_LINUX)
#include <signal.h>
#define SS_DEBUGBREAK() raise(SIGTRAP)
#else
#error "Platform doesn't support debugbreak yet!"
#endif
#define SS_ENABLE_ASSERTS
#else
#define SS_DEBUGBREAK()
#endif

#define SS_EXPAND_MACRO(x) x
#define SS_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << (x))

#define SS_BIND_EVENT_FN(fn) [this](auto&&... args) -> decltype(auto) { return this->fn(std::forward<decltype(args)>(args)...); }

namespace Snowstorm
{
	template <typename T>
	using Scope = std::unique_ptr<T>;

	template <typename T, typename... Args>
	constexpr Scope<T> CreateScope(Args&&... args)
	{
		return std::make_unique<T>(std::forward<Args>(args)...);
	}

	template <typename T>
	using Ref = std::shared_ptr<T>;

	template <typename T, typename... Args>
	constexpr Ref<T> CreateRef(Args&&... args)
	{
		return std::make_shared<T>(std::forward<Args>(args)...);
	}
}

#include "Snowstorm/Core/Log.h"
#include "Snowstorm/Core/Assert.h"
