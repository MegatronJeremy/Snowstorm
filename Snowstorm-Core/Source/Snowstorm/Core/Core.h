#pragma once

// Platform detection using predefined macros
#ifdef _WIN32
/* Windows x64/x86 */
#ifdef _WIN64
/* Windows x64 */
#define SS_PLATFORM_WINDOWS
#else
/* Windows x86 */
#error "x86 Builds are not supported!"
#endif
#elif defined(__APPLE__) || defined(__MACH__)
#include <TargetConditionals.h>
/* TARGET_OS_MAC exists on all the platforms
 * so we must check all of the (in this order)
 * to ensure that we're running on MAC
 * and not some other Apple platform */
#if TARGET_IPHONE_SIMULATOR == 1
#error "IOS simulator is not supported!"
#elif TARGET_OS_IPHONE == 1
#define SS_PLATFORM_IOS
#error "IOS is not supported!"
#elif TARGET_OS_MAC == 1
#define SS_PLATFORM_MACOS
#error "MacOS is not supported!"
#else
#error "Unknown Apple platform!"
#endif
/* We also have to check __ANDROID__ before __linux__
 * since android is based on the linux kernel
 * it has __linux__ defined */
#elif defined(__ANDROID__)
#define SS_PLATFORM_ANDROID
#error "Android is not supported!"
#elif defined(__linux__)
#define SS_PLATFORM_LINUX
#error "Linux is not supported!"
#else
/* Unknown compiler/platform */
#error "Unknown platform!"
#endif // End of platform detection

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
#endif

// DLL support
#ifdef SS_PLATFORM_WINDOWS
#if SS_DYNAMIC_LINK
#ifdef SS_BUILD_DLL
#define HAZEL_API __declspec(dllexport)
#else
#define HAZEL_API __declspec(dllimport)
#endif
#else
#define HAZEL_API
#endif
#endif // End of DLL support

#include <memory>

#ifdef SS_DEBUG
#define SS_ENABLE_ASSERTS
#endif

// TODO: Make this macro be able to take in no arguments except condition
#ifdef SS_ENABLE_ASSERTS
#define SS_ASSERT(x, ...) { if (!(x)) { SS_ERROR("Assertion Failed: {0}", __VA_ARGS__); SS_DEBUGBREAK(); } }
#define SS_CORE_ASSERT(x, ...) { if(!(x)) { SS_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); SS_DEBUGBREAK(); } }
#else
#define SS_ASSERT(x, ...)
#define SS_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << (x))

#define CONCAT(x, y) x##y
#define C(x, y) CONCAT(x, y)

#define SS_BIND_EVENT_FN(fn) [this](auto&&... args) { return this->fn(std::forward<decltype(args)>(args)...); }

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
