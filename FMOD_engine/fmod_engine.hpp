#pragma once
#include <fmod.hpp>

namespace fmod_engine {

class FMOD_Engine {
public:
	void PlaySound(const char* soundPath);
	void Release();
};

}