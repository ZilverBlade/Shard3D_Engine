#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "spdlog/sinks/rotating_file_sink.h"

namespace Shard3D {
	class LOGGER {
	public:
		static void init();
		inline static std::shared_ptr<spdlog::logger>& getDebugLogger() { return debugLogger; }

		inline static std::shared_ptr<spdlog::logger>& getInfoLogger() { return infoLogger; }
		inline static std::shared_ptr<spdlog::logger>& getWarnLogger() { return warnLogger; }
		inline static std::shared_ptr<spdlog::logger>& getErrorLogger() { return errorLogger; }
	private:
		inline static std::shared_ptr<spdlog::logger> debugLogger;

		inline static std::shared_ptr<spdlog::logger> infoLogger;
		inline static std::shared_ptr<spdlog::logger> warnLogger;
		inline static std::shared_ptr<spdlog::logger> errorLogger;
	};
}

#define SHARD3D_LOG(...)	Shard3D::LOGGER::getDebugLogger()->trace(__VA_ARGS__)
#define SHARD3D_INFO(...)	Shard3D::LOGGER::getInfoLogger()->trace(__VA_ARGS__)
#define SHARD3D_WARN(...)	Shard3D::LOGGER::getWarnLogger()->warn(__VA_ARGS__)
#define SHARD3D_ERROR(...)	Shard3D::LOGGER::getErrorLogger()->error(__VA_ARGS__)
#define SHARD3D_FATAL(...)	Shard3D::LOGGER::getErrorLogger()->critical(__VA_ARGS__)