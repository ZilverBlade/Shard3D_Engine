#pragma once
#include <fmod.hpp>

//documentation https://www.fmod.com/resources/documentation-api?version=2.01&page=core-api.html

namespace fmod_engine {

class FMOD_Core_Engine {
public:
	void PlaySoundFile(const char* soundPath);

	void Update();
	void UpdateVolume(float volume);
	void UpdatePitch(float pitch);

	void Release();
};
}