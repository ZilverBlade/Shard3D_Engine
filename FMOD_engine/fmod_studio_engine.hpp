#pragma once
#include <fmod.hpp>

//documentation https://www.fmod.com/resources/documentation-api?version=2.01&page=studio-api.html

namespace fmod_engine {

	class FMOD_Studio_Engine {
	public:
		void PlayBankEvent(const char* desktopPath, const char* bankName, const char* eventName);
		void PlayBankEvent(const char* desktopPath, const char* bankName, const char* eventName, const char* paramName, float paramVal);

		void Update();
		void UpdateParameter(const char* paramName, float paramVal, bool log = true);

		void Release();
	};
}