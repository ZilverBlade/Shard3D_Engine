#pragma once
#include <iostream>
#include <chrono>
namespace Shard3D {
	class Log {
	public:
		void logString(const char* log, bool timestamp = true, bool createNewline = true) {
			if (createNewline = true){
				if (timestamp = false) std::cout << log << "\n"; return;
			} else std::cout << log; return;

			std::cout << "[" << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << "] " << log << "\n";
		}
	};
}