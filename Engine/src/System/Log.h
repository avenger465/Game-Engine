#pragma once
#include "Base.h"

#pragma warning(push, 0)
#include <spdlog\spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace Engine
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

#define E_CORE_TRACE(...)    ::Engine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define E_CORE_INFO(...)     ::Engine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define E_CORE_WARN(...)     ::Engine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define E_CORE_ERROR(...)    ::Engine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define E_CORE_CRITICAL(...) ::Engine::Log::GetCoreLogger()->critical(__VA_ARGS__)
