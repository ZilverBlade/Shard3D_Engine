#pragma once
#include <fmod.hpp>

namespace fmod_engine {

class FMOD_Engine {
public:
	void PlayBankEvent(const char* desktopPath, const char* bankName, const char* eventName);
	void PlayBankEvent(const char* desktopPath, const char* bankName, const char* eventName, const char* paramName, float paramVal);

	void PlaySoundFile(const char* soundPath);

	void UpdateSoundFile();
	void UpdateSoundFile(float volume, float pitch);

	void ReleaseCore();
	void ReleaseStudio();
};
}