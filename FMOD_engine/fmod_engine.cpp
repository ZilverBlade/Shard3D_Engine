#include "fmod_engine.hpp"
#include <cstddef>

#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <iostream>

#pragma warning(disable : 4996)

namespace fmod_engine {


    FMOD_RESULT result;
    FMOD_RESULT resultstudio;
    FMOD::Studio::System* studiosystem = NULL;
    FMOD::System* system = NULL;
    
    FMOD::Sound* sound0;
    FMOD::Channel* channel = 0;

        void FMOD_Engine::PlaySoundFile(const char* soundPath) {   
            result = FMOD::System_Create(&system);

            result = system->init(512, 0, 0);
            result = system->createSound(soundPath, FMOD_DEFAULT, 0, &sound0);
            result = system->playSound(sound0, 0, false, &channel);

            result = system->update();

            std::cout << "Playing sound file: " << soundPath << std::endl;

        }

        void FMOD_Engine::UpdateSoundFile(){
            result = system->update();
        }
        void FMOD_Engine::UpdateSoundFile(float volume /*from 0 to 1*/, float pitch /*relative octaves (0.5 = half, 2 = double)*/) {
            result = channel->setVolume(volume);
            result = channel->setPitch(pitch);
            result = system->update();
        }

        void FMOD_Engine::PlayBankEvent(const char* desktopPath, const char* bankName, const char* eventName) {
            resultstudio = FMOD::Studio::System::create(&studiosystem); // Create the Studio System object.
            resultstudio = studiosystem->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);

            FMOD::Studio::Bank* masterBank = NULL;

            //combine the master bank path with the desktop path
            char* mbank = (char*)(calloc(strlen(desktopPath) + strlen("/Master.bank") - 1, 1));
            strncpy(mbank, desktopPath, strlen(desktopPath));
            strncat(mbank, "/Master.bank", strlen("/Master.bank"));

            studiosystem->loadBankFile((const char*)(mbank), FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);

            std::cout << "Loaded Master bank file: " << mbank << std::endl;

            FMOD::Studio::Bank* stringsBank = NULL;

            //combine the master string bank path with the desktop path
            char* mstrbank = (char*)(calloc(strlen(desktopPath) + strlen("/Master.strings.bank") - 1, 1));
            strncpy(mstrbank, desktopPath, strlen(desktopPath));
            strncat(mstrbank, "/Master.strings.bank", strlen("/Master.strings.bank"));

            studiosystem->loadBankFile((const char*)(mstrbank), FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank);

            std::cout << "Loaded Master strings file: " << mstrbank << std::endl;

            FMOD::Studio::Bank* sfxBank = NULL;

            char* banknm = (char*)(calloc(strlen(bankName), 1));
            strncpy(banknm, "/", 1);
            strncat(banknm, bankName, strlen(bankName));

            char* bankpth = (char*)(calloc(strlen(desktopPath) + strlen((const char*)banknm) - 1, 1));
            strncpy(bankpth, desktopPath, strlen(desktopPath));
            strncat(bankpth, (const char*)banknm, strlen((const char*)banknm));

            studiosystem->loadBankFile((const char*)bankpth, FMOD_STUDIO_LOAD_BANK_NORMAL, &sfxBank);

            std::cout << "Loaded bank file: " << bankName << std::endl;

            FMOD::Studio::EventDescription* eventDescription = NULL;
            studiosystem->getEvent(eventName, &eventDescription);

            FMOD::Studio::EventInstance* eventInstance = NULL;
            eventDescription->createInstance(&eventInstance);

            std::cout << "Instanced event: " << eventName << std::endl;

            eventInstance->start();

            resultstudio = studiosystem->update();
        }

        void FMOD_Engine::PlayBankEvent(const char* desktopPath, const char* bankName, const char* eventName, const char* paramName, float paramVal) {
            resultstudio = FMOD::Studio::System::create(&studiosystem); // Create the Studio System object.
            resultstudio = studiosystem->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);

            FMOD::Studio::Bank* masterBank = NULL;

            //combine the master bank path with the desktop path
            char* mbank = (char*)(calloc(strlen(desktopPath) + strlen("/Master.bank") - 1, 1));
            strncpy(mbank, desktopPath, strlen(desktopPath));
            strncat(mbank, "/Master.bank", strlen("/Master.bank"));

            studiosystem->loadBankFile((const char*)(mbank), FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);

            std::cout << "Loaded Master bank file: " << mbank << std::endl;

            FMOD::Studio::Bank* stringsBank = NULL;

            //combine the master string bank path with the desktop path
            char* mstrbank = (char*)(calloc(strlen(desktopPath) + strlen("/Master.strings.bank") - 1, 1));
            strncpy(mstrbank, desktopPath, strlen(desktopPath));
            strncat(mstrbank, "/Master.strings.bank", strlen("/Master.strings.bank"));

            studiosystem->loadBankFile((const char*)(mstrbank), FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank);

            std::cout << "Loaded Master strings file: " << mstrbank << std::endl;

            FMOD::Studio::Bank* sfxBank = NULL;


            char* banknm = (char*)(calloc(strlen(bankName), 1));
            strncpy(banknm, "/", 1);
            strncat(banknm, bankName, strlen(bankName));

            char* bankpth = (char*)(calloc(strlen(desktopPath) + strlen((const char*)banknm) - 1, 1));
            strncpy(bankpth, desktopPath, strlen(desktopPath));
            strncat(bankpth, (const char*)banknm, strlen((const char*)banknm));

            studiosystem->loadBankFile((const char*)bankpth, FMOD_STUDIO_LOAD_BANK_NORMAL, &sfxBank);

            std::cout << "Loaded bank file: " << bankName << std::endl;

            FMOD::Studio::EventDescription* eventDescription = NULL;
            studiosystem->getEvent(eventName, &eventDescription);

            FMOD::Studio::EventInstance* eventInstance = NULL;
            eventDescription->createInstance(&eventInstance);

            std::cout << "Instanced event: " << eventName << std::endl;

            eventInstance->setParameterByName(paramName, paramVal, false);

            std::cout << "Parameter '" << paramName << "' set with value " << paramVal << std::endl;

            eventInstance->start();

            resultstudio = studiosystem->update();
        }

        void FMOD_Engine::ReleaseCore() {
            result = system->release();
        }

        void FMOD_Engine::ReleaseStudio() {
            resultstudio = studiosystem->release();
        }
}