#include "fmod_studio_engine.hpp"
#include <cstddef>

#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <iostream>

#include "../utils/definitions.hpp"

#pragma warning(disable : 4996)

namespace fmod_engine {

    FMOD_RESULT result_s;
    FMOD::Studio::System* system = NULL;
  
    FMOD::Studio::EventDescription* eventDescription = NULL;
    FMOD::Studio::EventInstance* eventInstance = NULL;

    void FMOD_Studio_Engine::PlayBankEvent(const char* desktopPath, const char* bankName, const char* eventName) {
        result_s = FMOD::Studio::System::create(&system); // Create the Studio System object.
        result_s = system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);

        FMOD::Studio::Bank* masterBank = NULL;

        //combine the master bank path with the desktop path
        char* mbank = (char*)(calloc(strlen(desktopPath) + strlen(FMOD_MASTER_BANK) - 1, 1));
        strncpy(mbank, desktopPath, strlen(desktopPath));
        strncat(mbank, FMOD_MASTER_BANK, strlen(FMOD_MASTER_BANK));

        system->loadBankFile((const char*)(mbank), FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);

        std::cout << "Loaded Master bank file: " << mbank << std::endl;

        FMOD::Studio::Bank* stringsBank = NULL;

        //combine the master string bank path with the desktop path
        char* mstrbank = (char*)(calloc(strlen(desktopPath) + strlen(FMOD_MASTER_STRINGS_BANK) - 1, 1));
        strncpy(mstrbank, desktopPath, strlen(desktopPath));
        strncat(mstrbank, FMOD_MASTER_STRINGS_BANK, strlen(FMOD_MASTER_STRINGS_BANK));

        system->loadBankFile((const char*)(mstrbank), FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank);

        std::cout << "Loaded Master strings file: " << mstrbank << std::endl;

        FMOD::Studio::Bank* sfxBank = NULL;

        char* banknm = (char*)(calloc(strlen(bankName), 1));
        strncpy(banknm, "/", 1);
        strncat(banknm, bankName, strlen(bankName));

        char* bankpth = (char*)(calloc(strlen(desktopPath) + strlen((const char*)banknm) - 1, 1));
        strncpy(bankpth, desktopPath, strlen(desktopPath));
        strncat(bankpth, (const char*)banknm, strlen((const char*)banknm));

        system->loadBankFile((const char*)bankpth, FMOD_STUDIO_LOAD_BANK_NORMAL, &sfxBank);

        std::cout << "Loaded bank file: " << bankName << std::endl;

        system->getEvent(eventName, &eventDescription);

        eventDescription->createInstance(&eventInstance);

        std::cout << "Instanced event: " << eventName << std::endl;

        eventInstance->start();

        result_s = system->update();
    }

    void FMOD_Studio_Engine::PlayBankEvent(const char* desktopPath, const char* bankName, const char* eventName, const char* paramName, float paramVal) {
        result_s = FMOD::Studio::System::create(&system); // Create the Studio System object.
        result_s = system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);

        FMOD::Studio::Bank* masterBank = NULL;

        //combine the master bank path with the desktop path
        char* mbank = (char*)(calloc(strlen(desktopPath) + strlen(FMOD_MASTER_BANK) - 1, 1));
        strncpy(mbank, desktopPath, strlen(desktopPath));
        strncat(mbank, FMOD_MASTER_BANK, strlen(FMOD_MASTER_BANK));

        system->loadBankFile((const char*)(mbank), FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);

        std::cout << "Loaded Master bank file: " << mbank << std::endl;

        FMOD::Studio::Bank* stringsBank = NULL;

        //combine the master string bank path with the desktop path
        char* mstrbank = (char*)(calloc(strlen(desktopPath) + strlen(FMOD_MASTER_STRINGS_BANK) - 1, 1));
        strncpy(mstrbank, desktopPath, strlen(desktopPath));
        strncat(mstrbank, FMOD_MASTER_STRINGS_BANK, strlen(FMOD_MASTER_STRINGS_BANK));

        system->loadBankFile((const char*)(mstrbank), FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank);

        std::cout << "Loaded Master strings file: " << mstrbank << std::endl;

        FMOD::Studio::Bank* sfxBank = NULL;

        char* banknm = (char*)(calloc(strlen(bankName), 1));
        strncpy(banknm, "/", 1);
        strncat(banknm, bankName, strlen(bankName));

        char* bankpth = (char*)(calloc(strlen(desktopPath) + strlen((const char*)banknm) - 1, 1));
        strncpy(bankpth, desktopPath, strlen(desktopPath));
        strncat(bankpth, (const char*)banknm, strlen((const char*)banknm));

        system->loadBankFile((const char*)bankpth, FMOD_STUDIO_LOAD_BANK_NORMAL, &sfxBank);

        std::cout << "Loaded bank file: " << bankName << std::endl;

        system->getEvent(eventName, &eventDescription);

        eventDescription->createInstance(&eventInstance);

        std::cout << "Instanced event: " << eventName << std::endl;

        eventInstance->setParameterByName(paramName, paramVal, false);

        std::cout << "Parameter '" << paramName << "' set with value " << paramVal << std::endl;

        eventInstance->start();

        result_s = system->update();
    }

    void FMOD_Studio_Engine::Update() {
        result_s = system->update();
    } 

    void FMOD_Studio_Engine::UpdateParameter(const char* paramName, float paramVal, bool log /*disable if defining real time to prevent log spam*/) {
        eventInstance->setParameterByName(paramName, paramVal, false);

        if (log == true) {
            std::cout << "Parameter '" << paramName << "' set with value " << paramVal << std::endl;
        }
        
        result_s = system->update();
    }


    void FMOD_Studio_Engine::Release() {
        result_s = system->release();
    }
}