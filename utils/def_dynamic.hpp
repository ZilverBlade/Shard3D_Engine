#pragma once
 
#ifdef _WIN32
#include <WinUser.h>
#include <consoleapi3.h>
#endif
#include "definitions.hpp"
namespace Shard3D {
	class DYNAMICDEF {
	public:
		_S3D_GVAR void toggleConsole() {
#ifdef _WIN32
			if (IsWindowVisible(GetConsoleWindow()) == TRUE) {
				ShowWindowAsync(GetConsoleWindow(), SW_HIDE);
				return;
			}
			if (IsWindowVisible(GetConsoleWindow()) == FALSE)
				ShowWindowAsync(GetConsoleWindow(), SW_SHOW);
#endif
		}


	};
}

#define SHARD3D_TOGGLECONSOLE()	Shard3D::DYNAMICDEF::toggleConsole()