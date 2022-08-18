#pragma once
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace Shard3D {
	class LOGGER {
	public:
		static void init();
		static void logFatal(const std::string& message);

		static inline std::shared_ptr<spdlog::logger>& getDebugLogger() { return debugLogger; }
		static inline std::shared_ptr<spdlog::logger>& getInfoLogger() { return infoLogger; }
		static inline std::shared_ptr<spdlog::logger>& getWarnLogger() { return warnLogger; }
		static inline std::shared_ptr<spdlog::logger>& getErrorLogger() { return errorLogger; }
	private:
		static inline std::shared_ptr<spdlog::logger> debugLogger;
		static inline std::shared_ptr<spdlog::logger> infoLogger;
		static inline std::shared_ptr<spdlog::logger> warnLogger;
		static inline std::shared_ptr<spdlog::logger> errorLogger;
	};
}
/* Throw a message notifying that a function has not been implemented.
*/
#define SHARD3D_NOIMPL		Shard3D::LOGGER::getErrorLogger()->error("function not implemented!")
#ifndef NDEBUG
/* Log for debugging, doesnt log in release/deploy mode, so use conservatively.
*/
#define SHARD3D_LOG(...)	Shard3D::LOGGER::getDebugLogger()->trace(__VA_ARGS__)
#elif NDEBUG
#define SHARD3D_LOG(...)
#endif
/* Log information, for notifying what may happen, like asset loading.
*/
#define SHARD3D_INFO(...)	Shard3D::LOGGER::getInfoLogger()->trace(__VA_ARGS__)
/* Log a warning, such as for when something unexpected happens, and might cause undefined behaviour, but might function normally.
*/
#define SHARD3D_WARN(...)	Shard3D::LOGGER::getWarnLogger()->warn(__VA_ARGS__)
/* Log an error, such as for when something goes wrong, and has a chance of causing undefined behaviour.
*/
#define SHARD3D_ERROR(...)	Shard3D::LOGGER::getErrorLogger()->error(__VA_ARGS__)
/* Log a fatal error, also shows a message box as well as an exception, ending the program
(only use for a critical that will cause undefined behaviour).
Unlike the other logging macros, this only takes a string value.
*/ 
#define SHARD3D_FATAL(...)	Shard3D::LOGGER::logFatal(__VA_ARGS__)

/*
* Assert a statement, also shows a message box as well as an exception, ending the program
(use for catching potential mistakes in debugging).
Unlike the other logging macros, this only takes a string value.
*/
#ifndef SHARD3D_NO_ASSERT
#define SHARD3D_ASSERT(__EXPRESSION__) if (!(__EXPRESSION__)) Shard3D::LOGGER::logFatal("Assertion failed! '"#__EXPRESSION__"'")
#elif
#define SHARD3D_ASSERT(__EXPRESSION__, __MESSAGE__)
#endif