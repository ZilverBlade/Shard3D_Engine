#include "fmod_engine.hpp"
#include <cstddef>

#include <fmod.hpp>
#include <fmod_studio.hpp>
#include <iostream>
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

        void FMOD_Engine::PlayBankAction(const char* masterBankPath, const char* masterStringBankPath, const char* bankPath, const char* eventName) {
            resultstudio = FMOD::Studio::System::create(&studiosystem); // Create the Studio System object.
            resultstudio = studiosystem->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);

            FMOD::Studio::Bank* masterBank = NULL;
            studiosystem->loadBankFile(masterBankPath, FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);

            std::cout << "Loaded Master bank file: " << masterBankPath << std::endl;

            FMOD::Studio::Bank* stringsBank = NULL;
            studiosystem->loadBankFile(masterStringBankPath, FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank);

            std::cout << "Loaded Master strings file: " << masterStringBankPath << std::endl;

            FMOD::Studio::Bank* sfxBank = NULL;
            studiosystem->loadBankFile(bankPath, FMOD_STUDIO_LOAD_BANK_NORMAL, &sfxBank);

            std::cout << "Loaded bank file: " << bankPath << std::endl;

            FMOD::Studio::EventDescription* eventDescription = NULL;
            studiosystem->getEvent(eventName, &eventDescription);

            FMOD::Studio::EventInstance* eventInstance = NULL;
            eventDescription->createInstance(&eventInstance);

            std::cout << "Instanced event: " << eventName << std::endl;

            eventInstance->start();

            resultstudio = studiosystem->update();
        }

        void FMOD_Engine::PlayBankAction(const char* masterBankPath, const char* masterStringBankPath, const char* bankPath, const char* eventName, const char* paramName, float paramVal) {
            resultstudio = FMOD::Studio::System::create(&studiosystem); // Create the Studio System object.
            resultstudio = studiosystem->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, 0);

            FMOD::Studio::Bank* masterBank = NULL;
            studiosystem->loadBankFile(masterBankPath, FMOD_STUDIO_LOAD_BANK_NORMAL, &masterBank);

            std::cout << "Loaded Master bank file: " << masterBankPath << std::endl;

            FMOD::Studio::Bank* stringsBank = NULL;
            studiosystem->loadBankFile(masterStringBankPath, FMOD_STUDIO_LOAD_BANK_NORMAL, &stringsBank);

            std::cout << "Loaded Master strings file: " << masterStringBankPath << std::endl;

            FMOD::Studio::Bank* sfxBank = NULL;
            studiosystem->loadBankFile(bankPath, FMOD_STUDIO_LOAD_BANK_NORMAL, &sfxBank);

            std::cout << "Loaded bank file: " << bankPath << std::endl;

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