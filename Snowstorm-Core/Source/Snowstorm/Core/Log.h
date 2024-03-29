#pragma once

#include "Snowstorm/Core/Base.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"

// This ignores all warnings raised inside External headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Snowstorm
{
	class Log
	{
	public:
		static void Init();

		static Ref<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
		static Ref<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

	private:
		static Ref<spdlog::logger> s_CoreLogger;
		static Ref<spdlog::logger> s_ClientLogger;
	};
}

template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector)
{
	return os << glm::to_string(vector);
}

template <typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix)
{
	return os << glm::to_string(matrix);
}

template <typename OStream, typename T, glm::qualifier Q>
OStream& operator<<(OStream& os, glm::qua<T, Q> quaternion)
{
	return os << glm::to_string(quaternion);
}

// Core log macros
#define SS_CORE_TRACE(...)    ::Snowstorm::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define SS_CORE_INFO(...)     ::Snowstorm::Log::GetCoreLogger()->info(__VA_ARGS__)
#define SS_CORE_WARN(...)     ::Snowstorm::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define SS_CORE_ERROR(...)    ::Snowstorm::Log::GetCoreLogger()->error(__VA_ARGS__)
#define SS_CORE_CRITICAL(...) ::Snowstorm::Log::GetCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define SS_TRACE(...)         ::Snowstorm::Log::GetClientLogger()->trace(__VA_ARGS__)
#define SS_INFO(...)          ::Snowstorm::Log::GetClientLogger()->info(__VA_ARGS__)
#define SS_WARN(...)          ::Snowstorm::Log::GetClientLogger()->warn(__VA_ARGS__)
#define SS_ERROR(...)         ::Snowstorm::Log::GetClientLogger()->error(__VA_ARGS__)
#define SS_CRITICAL(...)      ::Snowstorm::Log::GetClientLogger()->critical(__VA_ARGS__)
