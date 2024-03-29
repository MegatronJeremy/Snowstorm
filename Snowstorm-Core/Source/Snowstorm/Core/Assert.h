#pragma once

#include "Snowstorm/Core/Base.h"
#include "Snowstorm/Core/Log.h"
#include <filesystem>

#ifdef SS_ENABLE_ASSERTS

// Alternatively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
#define SS_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { SS##type##ERROR(msg, __VA_ARGS__); SS_DEBUGBREAK(); } }
#define SS_INTERNAL_ASSERT_WITH_MSG(type, check, ...) SS_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
#define SS_INTERNAL_ASSERT_NO_MSG(type, check) SS_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", SS_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

#define SS_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
#define SS_INTERNAL_ASSERT_GET_MACRO(...) SS_EXPAND_MACRO( SS_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, SS_INTERNAL_ASSERT_WITH_MSG, SS_INTERNAL_ASSERT_NO_MSG) )

// Currently accepts at least the condition and one additional parameter (the message) being optional
#define SS_ASSERT(...) SS_EXPAND_MACRO( SS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
#define SS_CORE_ASSERT(...) SS_EXPAND_MACRO( SS_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
#define SS_ASSERT(...)
#define SS_CORE_ASSERT(...)
#endif
