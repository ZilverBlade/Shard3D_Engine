#pragma once
#include <fmod.hpp>

namespace fmod_engine {

class FMOD_Engine {
public:
	void PlayBankAction(const char* masterBankPath, const char* masterStringBankPath, const char* bankPath, const char* eventName);
	void PlayBankAction(const char* masterBankPath, const char* masterStringBankPath, const char* bankPath, const char* eventName, const char* paramName, float paramVal);
	//void PlayBankAction(const char* masterBankPath, const char* masterStringBankPath, const char* bankPath, const char* eventName, const char* paramLblName, const char* paramVal);
	void PlaySoundFile(const char* soundPath);
	void ReleaseCore();
	void ReleaseStudio();
};

}