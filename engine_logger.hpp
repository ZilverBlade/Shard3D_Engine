#pragma once
#include <iostream>
#include <chrono>
namespace Shard3D {
	class Log {
	public:
		void logString(const char* log, bool timestamp = true, bool createNewline = true) {
			if (createNewline == true){
				if (timestamp == false) { std::cout << log << "\n"; return; }
			}
			else {
				if (timestamp == false) { std::cout << log; return; }
			}

			std::cout << "[" << "12:34:56.789" << "] " << log << "\n";
		}
	};
}