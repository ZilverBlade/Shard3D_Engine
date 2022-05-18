#pragma once
#include <iostream>
#include <chrono>
namespace Shard3D {
	class Log {
		void logString(const char* log, bool timestamp = true) {
			if (timestamp = false) std::cout << log << "\n"; return;
	
			std::cout << "[" << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << "] " << log << "\n";
		}
	};
}