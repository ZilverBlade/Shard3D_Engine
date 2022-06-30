#include "engine_logger.hpp"
#include <iostream>
#include "utils/dialogs.h"
namespace Shard3D {
	void LOGGER::init() {
		spdlog::set_pattern("%^[%T] %n: %v%$");
		
		debugLogger = spdlog::stdout_color_mt("DEBUG");
		debugLogger->set_level(spdlog::level::trace);
		
		infoLogger = spdlog::stdout_color_mt("INFO");
		infoLogger->set_level(spdlog::level::trace);
		
		warnLogger = spdlog::stdout_color_mt("WARN");
		warnLogger->set_level(spdlog::level::warn);
		
		errorLogger = spdlog::stdout_color_mt("ERROR");
		errorLogger->set_level(spdlog::level::err);
	}
	void LOGGER::logFatal(std::shared_ptr<spdlog::logger>& lgr, const std::string& message) {
		lgr->critical(message);
		MessageDialogs::show(message.c_str(), "SHARD3D FATAL!!!", MessageDialogs::DialogOptions::OPTICONERROR);
		throw std::runtime_error(message);
	}

}