#include "fmod_studio_engine.hpp"
#include <cstddef>

#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <iostream>

#pragma warning(disable : 4996)

namespace fmod_engine {

    FMOD_RESULT results;
    FMOD::Studio::System* systems = NULL;
  
    void FMOD_Studio_Engine::PlayBankEvent(const char* desktopPath, const char* bankName, const char* eventName) {
        results = FMOD::Studio::System::create(&system); // Create the Studio System object.
        results = system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);

        FMOD::Studio::Bank* masterBank = NULL;

        //combine the master bank path with the desktop path
        char* mbank = (char*)(calloc(strlen(desktopPath) + strlen("/Master.bank") - 1, 1));
        strncpy(mbank, desktopPath, strlen(desktopPath));
        strncat(mbank, "/Master.bank", strlen("/Master.bank"));

        system->loadBankFile((const char*)(mbank), FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);

        std::cout << "Loaded Master bank file: " << mbank << std::endl;

        FMOD::Studio::Bank* stringsBank = NULL;

        //combine the master string bank path with the desktop path
        char* mstrbank = (char*)(calloc(strlen(desktopPath) + strlen("/Master.strings.bank") - 1, 1));
        strncpy(mstrbank, desktopPath, strlen(desktopPath));
        strncat(mstrbank, "/Master.strings.bank", strlen("/Master.strings.bank"));

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

        FMOD::Studio::EventDescription* eventDescription = NULL;
        system->getEvent(eventName, &eventDescription);
        system->getEvent(eventName, &eventDescription);

        FMOD::Studio::EventInstance* eventInstance = NULL;
        eventDescription->createInstance(&eventInstance);

        std::cout << "Instanced event: " << eventName << std::endl;

        eventInstance->start();

        results = system->update();
    }

    void FMOD_Studio_Engine::PlayBankEvent(const char* desktopPath, const char* bankName, const char* eventName, const char* paramName, float paramVal) {
        results = FMOD::Studio::System::create(&system); // Create the Studio System object.
        results = system->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);

        FMOD::Studio::Bank* masterBank = NULL;

        //combine the master bank path with the desktop path
        char* mbank = (char*)(calloc(strlen(desktopPath) + strlen("/Master.bank") - 1, 1));
        strncpy(mbank, desktopPath, strlen(desktopPath));
        strncat(mbank, "/Master.bank", strlen("/Master.bank"));

        system->loadBankFile((const char*)(mbank), FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);

        std::cout << "Loaded Master bank file: " << mbank << std::endl;

        FMOD::Studio::Bank* stringsBank = NULL;

        //combine the master string bank path with the desktop path
        char* mstrbank = (char*)(calloc(strlen(desktopPath) + strlen("/Master.strings.bank") - 1, 1));
        strncpy(mstrbank, desktopPath, strlen(desktopPath));
        strncat(mstrbank, "/Master.strings.bank", strlen("/Master.strings.bank"));

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

        FMOD::Studio::EventDescription* eventDescription = NULL;
        system->getEvent(eventName, &eventDescription);

        FMOD::Studio::EventInstance* eventInstance = NULL;
        eventDescription->createInstance(&eventInstance);

        std::cout << "Instanced event: " << eventName << std::endl;

        eventInstance->setParameterByName(paramName, paramVal, false);

        std::cout << "Parameter '" << paramName << "' set with value " << paramVal << std::endl;

        eventInstance->start();

        results = system->update();
    }

    void FMOD_Studio_Engine::Release() {
        results = system->release();
    }
}